# Motivation

Code for https://stackoverflow.com/q/79276745/16582

Code for https://stackoverflow.com/q/79283163/16582

# Problem

There is list of rectangles with different (known) widths and heights.

How to pack those rectangles around center point - like this:

![image](https://github.com/user-attachments/assets/3b1540d1-af9e-4201-b260-be5246ea5e75)

# Algorithm

Divide the area around the central point into 4 quadrants.

Divide the rectangles into 4 subsets of approximately equal total area.

Use a standard bin packing algorithm to pack each quadrant with its assigned rectangles.

A more detailed description is available here https://github.com/JamesBremner/so79276745/wiki/Algorithm

# Result

Here is how it looks with 50 boxes of random sizes

![image](https://github.com/user-attachments/assets/926e7808-bbc3-4b26-85f3-1f22ad02ff40)



