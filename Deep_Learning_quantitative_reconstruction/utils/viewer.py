# -*- encoding: utf-8 -*-
'''
@File    :   viewer.py
@Contact :   913799761@qq.com
@License :   (C)Copyright 2020-2021

@Modify Time      @Author    @Version    @Desciption
------------      -------    --------    -----------
2021/7/23 6:17 下午   haitao      0.0         None
'''
import os
import os.path as osp
import numpy as np
from .tools import mkdir
import matplotlib.pyplot as plt
import joblib
import scipy.io as scio
from collections import OrderedDict

class Viewer(object):
    def __init__(self, root):
        """
        :param root: the root path that will be saved
        """
        super(Viewer, self).__init__()
        self.save_root = root

        self.val_dir_name = "val"
        self.test_dir_name = "test"
        self.visdom_dir_name = "visdom"

        self.val_path = ""
        self.test_path = ""
        self.visdom_path = ""

    def set_root(self, root):
        self.root = root

    def check_all(self):
        mkdir(self.save_root)

        self.val_path = osp.join(self.save_root, self.val_dir_name)
        self.test_path = osp.join(self.save_root, self.test_dir_name)
        self.visdom_path = osp.join(self.save_root, self.visdom_dir_name)

        mkdir(self.val_path)
        mkdir(self.test_path)
        mkdir(self.visdom_path)

    def print_current_losses(self, losses, epoch ,inter_size):
        message = '[epoch: %d] ' % (epoch)
        for k, v in losses.items():
            message += '%s: %.6f |' % (k, v/inter_size)
        # print the message
        print(message)

    def print_val_losses(self, losses, inter_size):
        message = '[test ] '
        for k, v in losses.items():
            message += '%s: %.6f |' % (k, v/inter_size)
        # print the message
        print(message)

    def map2mat(self, visuals: OrderedDict, objdtype: str, epoch=None):

        inputs = visuals["inputs"]
        labels = visuals["labels"]
        pred_map = visuals["pred_map"]

        names = visuals["names"]

        # map_psnr = visuals["map_psnr"]
        # map_ssim = visuals["map_ssim"]

        batch_size = labels.shape[0]
        current_dir = osp.join(self.test_path, objdtype)
        mkdir(current_dir)

        for index in range(batch_size):
            mat = OrderedDict()
            mat["inputs"] = inputs[index, ...]
            mat["labels"] = labels[index, ...]

            mat["pred_map"] = pred_map[index, ...]
            mat["name"] = names[index]

            # mat["map_psnr"] = map_psnr[index, ...]
            # mat["map_ssim"] = map_ssim[index, ...]

            if epoch is None:
                file_path = osp.join(current_dir, '%s.mat' % (names[index]))
            else:
                file_path = osp.join(current_dir, 'epoch%d_%s.mat' % (epoch, names[index]))
            scio.savemat(file_path, mat)
        return
    def real2mat(self, visuals: OrderedDict, objdtype: str, epoch=None):
        inputs = visuals["inputs"]
        pred_map = visuals["pred_map"]
        names = visuals["names"]

        batch_size = pred_map.shape[0]
        current_dir = osp.join(self.test_path, objdtype)
        mkdir(current_dir)
        for index in range(batch_size):
            mat = OrderedDict()
            mat["inputs"] = inputs[index, ...]
            mat["pred_map"] = pred_map[index, ...]

            if epoch is None:
                file_path = osp.join(current_dir, '%s.mat' % (names[index]))
            else:
                file_path = osp.join(current_dir, 'epoch%d_%s.mat' % (epoch, names[index]))
            scio.savemat(file_path, mat)
        return