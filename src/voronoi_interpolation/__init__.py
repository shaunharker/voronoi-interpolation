from ._voronoi_interpolation import voronoi_interpolation

import numpy as np
from PIL import Image
import matplotlib.pyplot as plt

def random_sites(shape, N):
    return np.random.randint(0, high=shape[:2], size=(N, 2), dtype=np.uint16)

def load_png_as_numpy(input_filename="input.png"):
    # Load input image as numpy array
    image = Image.open(input_filename)
    array = np.array(image)
    # Check if it's a 4-channel image and convert it to a 3-channel image by removing the alpha channel
    if array.shape[2] == 4:
        array = array[:, :, :3]
    return array

def save_numpy_to_png(array, output_filename="output.png"):
    # Convert numpy array to PIL image
    image = Image.fromarray(array)
    # Save PIL image as PNG
    image.save(output_filename)

def display_image(image_np, image_title='Image'):
    fig, ax = plt.subplots(figsize=(5, 5))
    ax.imshow(image_np)
    ax.set_title(image_title)
    ax.axis('off')
    plt.show()

def display_side_by_side(left_np, right_np, left_title="Input Image", right_title="Output Image"):
    # Display the two images side by side
    fig, axes = plt.subplots(1, 2, figsize=(10, 5))
    axes[0].imshow(left_np)
    axes[0].set_title(left_title)
    axes[0].axis('off')
    axes[1].imshow(right_np)
    axes[1].set_title(right_title)
    axes[1].axis('off')
    plt.show()