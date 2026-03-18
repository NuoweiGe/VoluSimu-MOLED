# -*- encoding: utf-8 -*-
'''
@File    :   croper.py
@Contact :   913799761@qq.com
@License :   (C)Copyright 2020-2021

@Modify Time      @Author    @Version    @Desciption
------------      -------    --------    -----------
2021/7/24 11:13 上午   haitao      0.0         None
'''
import random


class RandomCroper(object):
    def __init__(self, crop_size):
        super(RandomCroper, self).__init__()
        self.crop_size = crop_size

    def crop_one(self, image):
        old_shape = image.shape
        old_H = old_shape[0]
        old_W = old_shape[1]
        H = random.randint(0, old_H - self.crop_size)
        W = random.randint(0, old_W - self.crop_size)

        image = image[H: H + self.crop_size, W: W + self.crop_size, ...]

        return image

    def crop_two(self, image, label):
        old_shape = image.shape
        old_H = old_shape[0]
        old_W = old_shape[1]

        H = random.randint(0, old_H - self.crop_size)
        W = random.randint(0, old_W - self.crop_size)

        image = image[H: H + self.crop_size, W: W + self.crop_size, ...]
        label = label[H: H + self.crop_size, W: W + self.crop_size, ...]

        return [image, label]

    def crop_three(self, input_1, input_2, input_3):
        old_shape = input_1.shape
        old_H = old_shape[0]
        old_W = old_shape[1]

        H = random.randint(0, old_H - self.crop_size)
        W = random.randint(0, old_W - self.crop_size)

        input_1 = input_1[H: H + self.crop_size, W: W + self.crop_size, ...]
        input_2 = input_2[H: H + self.crop_size, W: W + self.crop_size, ...]
        input_3 = input_3[H: H + self.crop_size, W: W + self.crop_size, ...]
        return [input_1, input_2, input_3]

    def crop_four(self, input_1, input_2, input_3, input_4):
        old_shape = input_1.shape
        old_H = old_shape[0]
        old_W = old_shape[1]

        H = random.randint(0, old_H - self.crop_size)
        W = random.randint(0, old_W - self.crop_size)

        input_1 = input_1[H: H + self.crop_size, W: W + self.crop_size, ...]
        input_2 = input_2[H: H + self.crop_size, W: W + self.crop_size, ...]
        input_3 = input_3[H: H + self.crop_size, W: W + self.crop_size, ...]
        input_4 = input_4[H: H + self.crop_size, W: W + self.crop_size, ...]
        return [input_1, input_2, input_3, input_4]

    def crop_five(self, input_1, input_2, input_3, input_4, input_5):
        old_shape = input_1.shape
        old_H = old_shape[0]
        old_W = old_shape[1]

        H = random.randint(0, old_H - self.crop_size)

        W = random.randint(0, old_W - self.crop_size)

        input_1 = input_1[H: H + self.crop_size, W: W + self.crop_size, ...]
        input_2 = input_2[H: H + self.crop_size, W: W + self.crop_size, ...]
        input_3 = input_3[H: H + self.crop_size, W: W + self.crop_size, ...]
        input_4 = input_4[H: H + self.crop_size, W: W + self.crop_size, ...]
        input_5 = input_5[H: H + self.crop_size, W: W + self.crop_size, ...]
        return [input_1, input_2, input_3, input_4, input_5]