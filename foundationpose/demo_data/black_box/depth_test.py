import os
from PIL import Image
import numpy as np

def get_unique_values_in_grayscale_images(directory):
    unique_values = set()
    
    for filename in os.listdir(directory):
        if filename.endswith('.png') or filename.endswith('.jpg') or filename.endswith('.jpeg'):
            # Load the image
            img_path = os.path.join(directory, filename)
            image = Image.open(img_path).convert('L')  # Convert image to grayscale
            
            # Convert image to numpy array
            img_array = np.array(image)
            
            # Get unique values in the image and update the set
            unique_values.update(np.unique(img_array))
    
    return unique_values

# Specify the directory containing the grayscale images
directory = './demo_data/black_box2/depth'
unique_values = get_unique_values_in_grayscale_images(directory)
print("Unique values in grayscale images:", unique_values)

