# 2023-05-02-VoroinoiInterpolation

This module provides a method to perform Voronoi cell-based interpolation. 

def perform_voronoi_interpolation(input_image=None, num_points=None, sites=None,
                                  input_filename="", output_filename="",
                                  save_to_disk=False)

Parameters:

    input_image (numpy.ndarray, optional): The input image as a NumPy array.
    num_points (int, optional): The number of sites to be randomly generated. Ignored if sites is set.
    sites (list of tuples or numpy.ndarray, optional): A list of tuples or a NumPy array containing the (x, y) coordinates of the polygons centroids.
    input_filename (str, optional): The file path of the input image. Ignored if input_image is set.
    output_filename (str, optional): The file path to save the resulting image. Ignored if save_to_disk is set to False.
    save_to_disk (bool, optional): Whether to save the output image to the provided output_filename or not. Default is False.

Returns:
    numpy.ndarray: The output image with interpolated colors as a NumPy array.
```

## Installation

To build the package and install it in the local environment, execute:

```bash
$ 
$ cd build
$ cmake ..
$ make
$ cd ..
$ pip install .
```

To test the installed package in Python:

```python
import cv2
import numpy as np
import voronoi_interpolation

input_image = cv2.imread("input.png", cv2.IMREAD_COLOR)
num_points = 3000

output_image = voronoi_interpolation.perform_voronoi_interpolation(input_image=input_image, num_points=num_points)
cv2.imwrite("output.png", output_image)

## Example usage



For a Jupyter Notebook version, you can use the following code:

```python
%matplotlib inline
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image
from voronoi_interpolation import voronoi_interpolation

# Load the image and convert it to a NumPy array
input_image = np.array(Image.open("image.png"))

# Generate 3000 random Voronoi centroids
height, width, _ = input_image.shape
sites = np.random.randint(0, high=[height, width], size=(3000, 2), dtype=np.uint16)

# Perform Voronoi interpolation
output_image = voronoi_interpolation(input_image, sites)

# Save the output image to output.png
output_image_pil = Image.fromarray(output_image)
output_image_pil.save("output.png")

# Display the input and output images in the Notebook
plt.figure(figsize=(12, 6))
plt.subplot(1, 2, 1)
plt.imshow(input_image)
plt.title("Input Image")
plt.axis("off")
plt.subplot(1, 2, 2)
plt.imshow(output_image)
plt.title("Output Image with Voronoi Interpolation")
plt.axis("off")
plt.show()
```