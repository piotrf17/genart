// Utility functions for dealing with polygon effect images.

#ifndef GENART_POLY_UTIL_H
#define GENART_POLY_UTIL_H

#include <string>
#include <vector>

namespace poly {

class Polygon;
  
namespace output {
class PolygonImage;
}  // namespace output

// Convert between the vector of polygons used to represent a polygon
// image internally in code, to a more easy to serialize proto.
// TODO(piotrf): Do we need to actually expose the proto anywhere?
void PolygonProtoToVector(const output::PolygonImage& proto,
                          std::vector<Polygon>* vec);
void VectorToPolygonProto(const std::vector<Polygon>& vec,
                          output::PolygonImage* output);

// Save a polygon image to an SVG file.  In the future this may be expanded
// to more types of files.
void SaveImageToFile(const output::PolygonImage& output,
                     const std::string& filename);

}  // namespace poly

#endif
