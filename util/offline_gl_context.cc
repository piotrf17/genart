#include "util/offline_gl_context.h"

#include <glog/logging.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include "image/image.h"

using genart::image::Image;

namespace genart {
namespace util {

// Necessary state for a GLX Pixmap based offline rendering window.
struct OfflineGLContextState {
  Display* dpy;
  GLXPixmap glx_pixmap;
  GLXContext ctx;  
};

OfflineGLContext::OfflineGLContext(int width, int height)
    : width_(width),
      height_(height),
      state_(new OfflineGLContextState) {
  CHECK(Init());
}

OfflineGLContext::~OfflineGLContext() {
  CHECK(glXMakeCurrent(state_->dpy, None, nullptr));
  glXDestroyContext(state_->dpy, state_->ctx);
  XCloseDisplay(state_->dpy);
}

bool OfflineGLContext::Acquire() {
  return glXMakeCurrent(state_->dpy, state_->glx_pixmap, state_->ctx);
}

bool OfflineGLContext::Init() {
  // Open a display connection to the X server.
  state_->dpy = XOpenDisplay(nullptr);
  if (state_->dpy == nullptr) {
    LOG(ERROR) << "Failed to connect to the X server";
    return false;
  }
  int screen = DefaultScreen(state_->dpy);

  // Choose a single buffered visual.
  static int attrListSgl[] = {GLX_RGBA, GLX_RED_SIZE, 4,
                              GLX_GREEN_SIZE, 4,
                              GLX_BLUE_SIZE, 4,
                              GLX_DEPTH_SIZE, 16,
                              None};
  XVisualInfo* vi = glXChooseVisual(state_->dpy, screen, attrListSgl);

  // Create a pixmap for offline rendering.
  Pixmap pixmap = XCreatePixmap(state_->dpy,
                                RootWindow(state_->dpy, vi->screen),
                                width_, height_, vi->depth);
  state_->glx_pixmap = glXCreateGLXPixmap(state_->dpy, vi, pixmap);

  // Create a GLX context.
  state_->ctx = glXCreateContext(state_->dpy, vi, 0, GL_TRUE);
  if (!glXMakeCurrent(state_->dpy, state_->glx_pixmap, state_->ctx)) {
    LOG(ERROR) << "Failed to make direct pixmap context";
    return false;
  }

  LOG(INFO) << "Pixmap direct rendering: "
            << (glXIsDirect(state_->dpy, state_->ctx) ? "Yes" : "No");

  // TODO(piotrf): move opengl specific initialization somewhere else?
  // Create a viewport the size of the image.
  glViewport(0, 0, width_, height_);
  // Setup an orthographic projection.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width_, 0, height_, -1.0, 1.0);
  // Return to model view matrix mode.
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // Clear and set colors.
  glShadeModel(GL_FLAT);
  glClearColor(0.0, 0.0, 0.0, 1);
  glDisable(GL_DEPTH_TEST);
  glFlush();
  glClear(GL_COLOR_BUFFER_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  return true;
}

std::unique_ptr<Image> OfflineGLContext::GetImage() const {
  // Read the image bytes into a new image object.
  unsigned char* pixels = new unsigned char[3 * width_ * height_];

  // For some reason, openGL is ignoring the pack alignment parameter,
  // and insists on packing aligned to 4 bytes.  So we make a copy
  // and do the realignment here.
  if (width_ % 4 == 0) {
    glReadPixels(0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  } else {
    const int aligned_byte_width = (3 * width_ + (4 - 1)) & -4;
    unsigned char* aligned_pixels =
        new unsigned char[aligned_byte_width * height_];
    glReadPixels(0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE,
                 aligned_pixels);
    for (int i = 0; i < height_; ++i) {
      for (int j = 0; j < width_; ++j) {
        const int dealigned = 3 * (i * width_ + j);
        const int aligned = i * aligned_byte_width + 3 * j;
        pixels[dealigned + 0] = aligned_pixels[aligned + 0];
        pixels[dealigned + 1] = aligned_pixels[aligned + 1];
        pixels[dealigned + 2] = aligned_pixels[aligned + 2];
      }
    }
  }

  return std::unique_ptr<image::Image>(
      new image::Image(pixels, width_, height_));
}

}  // namespace util
}  // namespace genart
