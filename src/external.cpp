#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/centroid.h>
#include <CGAL/intersections.h>
#include <CGAL/point_generators_2.h>
#include <array>
#include <cstdint>
#include <map>
#include <limits>
#include <cmath>
#include <iostream>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> Delaunay;
typedef Delaunay::Point Point;

std::array<uint8_t, 3> barycentric_interpolation(
    const std::array<std::array<uint8_t, 3>, 3> &vertex_colors,
    const std::array<Point, 3> &triangle_vertices,
    const Point &query_point)
{
    double area_ABC = std::abs(CGAL::area(triangle_vertices[0], triangle_vertices[1], triangle_vertices[2]));
    double area_PBC = std::abs(CGAL::area(query_point, triangle_vertices[1], triangle_vertices[2]));
    double area_PCA = std::abs(CGAL::area(query_point, triangle_vertices[2], triangle_vertices[0]));
    double area_PAB = std::abs(CGAL::area(query_point, triangle_vertices[0], triangle_vertices[1]));

    double w_A = area_PBC / area_ABC;
    double w_B = area_PCA / area_ABC;
    double w_C = area_PAB / area_ABC;

    return std::array<uint8_t, 3>{static_cast<uint8_t>(w_A * vertex_colors[0][0] + w_B * vertex_colors[1][0] + w_C * vertex_colors[2][0]),
                                   static_cast<uint8_t>(w_A * vertex_colors[0][1] + w_B * vertex_colors[1][1] + w_C * vertex_colors[2][1]),
                                   static_cast<uint8_t>(w_A * vertex_colors[0][2] + w_B * vertex_colors[1][2] + w_C * vertex_colors[2][2])};
}

void calculate_average_colors(int rows, int cols,
    const std::vector<std::array<uint8_t, 3>> &input_image,
    const Delaunay &dt, std::map<Point, std::array<uint32_t, 3>> &vertex_to_cumulative_color,
    std::map<Point, uint32_t> &vertex_to_pixel_count)
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            Point query_point(i, j);
            auto face_handle = dt.locate(query_point);

            if (dt.is_infinite(face_handle))
            {
                continue;
            }

            Delaunay::Vertex_handle closest_vertex;
            double min_dist = std::numeric_limits<double>::max();
            for (int k = 0; k < 3; ++k)
            {
                auto vertex_handle = face_handle->vertex(k);
                Point pt = vertex_handle->point();
                double dist = CGAL::squared_distance(pt, query_point);
                if (dist < min_dist)
                {
                    min_dist = dist;
                    closest_vertex = vertex_handle;
                }
            }

            Point pt = closest_vertex->point();
            int idx = i * cols + j;
            vertex_to_cumulative_color[pt][0] += input_image[idx][0];
            vertex_to_cumulative_color[pt][1] += input_image[idx][1];
            vertex_to_cumulative_color[pt][2] += input_image[idx][2];
            vertex_to_pixel_count[pt]++;
        }
    }
}

void fill_output_image(int rows, int cols, const Delaunay &dt,
    const std::map<Point, std::array<uint8_t, 3>> &vertex_to_average_color,
    std::vector<std::array<uint8_t, 3>> &output_image)
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            Point query_point(i, j);
            auto face_handle = dt.locate(query_point);

            if (dt.is_infinite(face_handle))
            {
                continue;
            }

            std::array<std::array<uint8_t, 3>, 3> vertex_colors;
            std::array<Point, 3> triangle_vertices;
            for (int idx = 0; idx < 3; ++idx)
            {
                triangle_vertices[idx] = face_handle->vertex(idx)->point();
                vertex_colors[idx] = vertex_to_average_color.at(triangle_vertices[idx]);
            }

            int idx = i * cols + j;
            output_image[idx] = barycentric_interpolation(vertex_colors, triangle_vertices, query_point);
        }
    }
}

std::vector<std::array<uint8_t, 3>>
perform_voronoi_interpolation(
    const std::vector<std::array<uint8_t, 3>> &input_image,
    int rows,
    int cols,
    const std::vector<Point> &points)
{
    Delaunay dt;
    dt.insert(points.begin(), points.end());

    std::map<Point, std::array<uint32_t, 3>> vertex_to_cumulative_color;
    std::map<Point, uint32_t> vertex_to_pixel_count;
    calculate_average_colors(rows, cols, input_image, dt, vertex_to_cumulative_color, vertex_to_pixel_count);
    std::map<Point, std::array<uint8_t, 3>> vertex_to_average_color;
    for (const auto &[point, cumulative_color] : vertex_to_cumulative_color)
    {
        uint32_t pixel_count = vertex_to_pixel_count[point];
        vertex_to_average_color[point] = std::array<uint8_t, 3>{static_cast<uint8_t>(cumulative_color[0] / pixel_count),
                                                                 static_cast<uint8_t>(cumulative_color[1] / pixel_count),
                                                                 static_cast<uint8_t>(cumulative_color[2] / pixel_count)};
    }
    std::vector<std::array<uint8_t, 3>> output_image(rows * cols);
    fill_output_image(rows, cols, dt, vertex_to_average_color, output_image);
    return output_image;
}
