import cv2
import numpy as np
from pylab import *


def edge_detect(file_name, tresh_min, tresh_max):
    image = cv2.imread(file_name)
    im_bw = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)

    (thresh, im_bw) = cv2.threshold(im_bw, tresh_min, tresh_max, 0)
    cv2.imwrite('bw_' + file_name, im_bw)

    contours, hierarchy = cv2.findContours(im_bw, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    cv2.drawContours(image, contours, -1, (0, 255, 0), 3)
    cv2.imwrite('cnt_' + file_name, image)


def sum_abs(list1, list2):
    sum_abs = 0
    for i in range(len(list1)):
        if list1[i] > list2[i]:
            sum_abs += list1[i] - list2[i]
        else:
            sum_abs += list2[i] - list1[i]
    return sum_abs


def test(file_name):
    image = cv2.imread(file_name)
    im_bw = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)

    (thresh, im_bw) = cv2.threshold(im_bw, 40, 255, 0)

    contours, hierarchy = cv2.findContours(im_bw, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    max_area = 0
    new_contours = []
    for cnt in contours:
        area = cv2.contourArea(cnt)
        if area > max_area:
            max_area = area

    for cnt in contours:
        if cv2.contourArea(cnt) >= max_area - 3000:
            new_contours += [cnt]

    color_dict = {}
    color_num = 1
    flask_list = []
    norm_bound = 30
    for cnt in new_contours:
        x, y, w, h = cv2.boundingRect(cnt)
        column = image[y + 100: y + h: (h - 100) // 4, x + w // 2]

        fluid_list = []
        old_value = None
        for pixel in column:
            string = f"{pixel[0]}:{pixel[1]}:{pixel[2]}"

            if (abs((sum(pixel / 3) - pixel[0])) >= 8) or (abs((sum(pixel / 3) - pixel[1])) >= 8):

                if old_value is None:
                    for key in color_dict.keys():
                        key_values = np.array([int(val) for val in key.split(":")])
                        if sum(abs(key_values - pixel)) <= norm_bound:
                            string = key

                    if not color_dict.get(string):
                        color_dict[string] = color_num
                        color_num += 1
                    fluid_list += [color_dict[string]]
                    old_value = pixel

                else:
                    for key in color_dict.keys():
                        key_values = np.array([int(val) for val in key.split(":")])
                        if sum_abs(key_values, pixel) <= norm_bound:
                            string = key

                    if not color_dict.get(string):
                        color_dict[string] = color_num
                        color_num += 1
                    fluid_list += [color_dict[string]]

                    old_value = pixel

        flask_list += [fluid_list]

    checker = [0 for i in range(len(color_dict))]
    for flask in flask_list:
        for fluid in flask:
            checker[fluid-1] += 1

    for check in checker:
        if check != 4:
            print("error")

    file = open("./result.txt", 'w+')
    for flask in flask_list:
        file.write(f"{flask}\n")




test("test1.jpg")
