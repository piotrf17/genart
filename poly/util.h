// Utility functions for dealing with polygon effect images.

#ifndef GENART_POLY_UTIL_H
#define GENART_POLY_UTIL_H

#include <string>

namespace poly {

namespace output {
class PolygonImage;
}  // namespace output
  
void SaveImageToFile(const output::PolygonImage& output,
                     const std::string& filename);

}  // namespace poly

#endif
