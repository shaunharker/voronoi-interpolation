#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/centroid.h>
#include <CGAL/intersections.h>
#include <CGAL/point_generators_2.h>
#include <vector>
#include <array>
#include <cstdint>
#include <map>
#include <limits>
#include <cmath>


typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> Delaunay;
typedef Delaunay::Point Point;


std::vector<std::array<uint8_t, 3>>
perform_voronoi_interpolation(
    const std::vector<std::array<uint8_t, 3>> &input_image,
    int rows,
    int cols,
    const std::vector<Point> &points);


#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
namespace nb = nanobind;
using namespace nb::literals;


std::vector<std::array<uint8_t, 3>>
perform_voronoi_interpolation(
    const std::vector<std::array<uint8_t, 3>> &input_image,
    int rows,
    int cols,
    const std::vector<Point> &points);


// Wrapper function for performing Voronoi interpolation.
nb::ndarray<nb::numpy, uint8_t, nb::shape<nb::any, nb::any, 3>, nb::c_contig>
perform_voronoi_interpolation_py(
        nb::ndarray<nb::numpy, uint8_t, nb::shape<nb::any, nb::any, 3>, nb::c_contig > & input_image_nd,
        nb::ndarray<nb::numpy, uint16_t, nb::shape<nb::any, 2>, nb::c_contig> & sites)
{
    // Convert input ndarray
    size_t rows = input_image_nd.shape(0);
    size_t cols = input_image_nd.shape(1);
    std::vector<std::array<uint8_t, 3>> input_image(rows*cols);
    for (size_t i = 0; i < input_image_nd.shape(0); ++ i) {
        for (size_t j = 0; j < input_image_nd.shape(1); ++ j) {
            input_image[cols*i +j] = {input_image_nd(i,j,0), input_image_nd(i,j,1), input_image_nd(i,j,2)};
        }
    }
    
    // Convert from NumPy array to std::vector of points.
    std::vector<Point> points;
    for (size_t i = 0; i < sites.shape(0); ++i)
    {
        points.push_back(Point(sites(i, 0), sites(i, 1)));
    }

    // Call the Voronoi interpolation method.
    std::vector<std::array<uint8_t, 3>> output_image =
        perform_voronoi_interpolation(input_image, rows, cols, points);
    
    // Allocate memory for the uint8_t array
    uint8_t* data = new uint8_t[rows * cols * 3];

    // Copy the data from the vector<array<3>> to the uint8_t array
    for (size_t i = 0; i < output_image.size(); ++i)
    {
        data[i * 3]     = output_image[i][0];
        data[i * 3 + 1] = output_image[i][1];
        data[i * 3 + 2] = output_image[i][2];
    }
    
    // Delete 'data' when the 'owner' capsule expires
    nb::capsule owner(data, [](void *p) noexcept {
       delete[] (uint8_t *) p;
    });

    // Construct ndarray owning data
    auto output_image_nd = nb::ndarray<nb::numpy, uint8_t, nb::shape<nb::any, nb::any, 3>, nb::c_contig>(
        data, 3, (const size_t *)input_image_nd.shape_ptr(), owner);

    return output_image_nd;
}

NB_MODULE(_voronoi_interpolation, m)
{
    m.def("voronoi_interpolation", &perform_voronoi_interpolation_py,
          "input_image"_a,
          "sites"_a,
          "Perform Voronoi interpolation on an input image and return the output image with triangle interpolation."
            "\n\n"
            "Parameters:\n"
            "input_image (ndarray): An input color image as a 3D ndarray with shape (height, width, 3) and uint8 dtype.\n"
            "sites (ndarray): A 2D ndarray (shape (*, 2)) of (x, y) coordinates for Voronoi centroids, dtype should be uint16.\n\n"
            "Returns:\n"
            "output_image (ndarray): The output image with triangle interpolation as a 3D ndarray with shape (height, width, 3) and uint8 dtype.\n");
}
