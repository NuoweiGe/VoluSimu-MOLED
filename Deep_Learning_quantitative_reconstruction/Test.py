# -*- coding: UTF-8 -*-
'''
Created on Wed Oct 9 20:15:00 2019

@author: Qinqin Yang
'''
import os
import argparse
import scipy.io as matio

from network.R2AttUNet_single import Inference

from tools.evaluation import *
import numpy as np

from torch.utils import data
from torchvision import transforms as T

def load_data_Charles_test(image_path, config):
    #print(image_path)
    data_in = np.fromfile(image_path, dtype=np.float32)
    data_pairs = data_in.reshape(config.INPUT_H, config.INPUT_W, config.DATA_C)
    input_sets = 1*data_pairs[:, :, 0:config.INPUT_C]
    label_sets = data_pairs[:, :, :config.INPUT_C]

    return input_sets,label_sets


class ImageFolder(data.Dataset):
    """Load Variaty Chinese Fonts for Iterator. """

    def __init__(self, root, config, crop_key, mode='train'):
        """Initializes image paths and preprocessing module."""
        self.config = config
        self.root = root
        self.mode = mode
        self.crop_key = crop_key
        self.crop_size = config.CROP_SIZE
        self.image_dir = os.path.join(root, mode)

        self.image_paths = list(map(lambda x: os.path.join(self.image_dir, x), os.listdir(self.image_dir)))
        print("image count in {} path :{}".format(self.mode, len(self.image_paths)))
        self.image_paths.sort(reverse=True)

    def __getitem__(self, index):
        """Reads an image from a file and preprocesses it and returns."""
        image_path = self.image_paths[index]
        image, GT = load_data_Charles_test(image_path, self.config)

        # -----To Tensor------#
        Transform = T.ToTensor()
        image = Transform(image)
        GT = Transform(GT)

        return image, GT

    def __len__(self):
        """Returns the total number of font files."""
        return len(self.image_paths)

def get_loader(image_path, config, crop_key, num_workers, shuffle=True,mode='train'):
    """Builds and returns Dataloader."""

    dataset = ImageFolder(root=image_path, config=config, crop_key=crop_key, mode=mode)
    data_loader = data.DataLoader(dataset=dataset,
                                  batch_size=config.BATCH_SIZE,
                                  shuffle=shuffle,
                                  num_workers=num_workers)
    return data_loader

def v_test(config):
    #-----选择GPU-----#
    os.environ['CUDA_VISIBLE_DEVICES'] = config.GPU_NUM

    #-----使每次生成的随机数相同-----#
    np.random.seed(1)
    torch.manual_seed(1)

    # -----地址-----#
    model_dir = os.path.join(config.model_path, config.name+'/'+ config.name+ '_epoch_' +config.model_num + '.pth')
    if not os.path.exists(model_dir):
        print('Model not found, please check you path to model')
        print(model_dir)
        os._exit(0)
    if not os.path.exists(config.result_path):
        os.makedirs(config.result_path)

    #-----读取数据-----#
    test_batch = get_loader(config.data_dir, config, crop_key=False, num_workers=1, shuffle=False, mode=config.test_dir)

    #-----模型-----#
    net = Inference(config.INPUT_C,config.OUTPUT_C,config.FILTERS)

    if torch.cuda.is_available():
        net.cuda()

    #-----载入模型参数-----#
    mo=torch.load(model_dir)
    #print(mo)
    net.load_state_dict(torch.load(model_dir))
    print('Model parameters loaded!')

    # Setup device
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    #device = torch.device("cpu")
    # ********************************************test*****************************************************#
    net.eval()
    for i,(images, GT) in enumerate(test_batch):
        images = images.type(torch.FloatTensor)
        GT = GT.type(torch.FloatTensor)

        images = images.to(device)

        SR = net(images)  # forward

        if i == 0:
            OUT_test = SR.permute(0, 2, 3, 1).cpu().detach().numpy()
        else:
            OUT_test = np.concatenate((SR.permute(0, 2, 3, 1).cpu().detach().numpy(),OUT_test),axis=0)

    #-----保存为mat文件-----#
    print('.' * 30)
    print('OUT_test:', OUT_test.shape)
    print('.' * 30)
    matio.savemat(
        os.path.join(config.result_path, config.name + '_result_' + config.test_dir + '_08.mat'),
        {
            'output': OUT_test
        })
    print('Save result in ',config.name + '_result_' + config.test_dir + '.mat')
    print('.' * 30)
    print('Finished!')

if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    # experiment name
    parser.add_argument('--name', type=str, default='experiment')
    parser.add_argument('--data_dir', type=str, default='/datasets/')
    parser.add_argument('--GPU_NUM', type=str, default='7')

    # model hyper-parameters
    parser.add_argument('--INPUT_H', type=int, default=256)
    parser.add_argument('--INPUT_W', type=int, default=256)
    parser.add_argument('--INPUT_C', type=int, default=4)
    parser.add_argument('--OUTPUT_C', type=int, default=1)

    parser.add_argument('--DATA_C', type=int, default=6)
    parser.add_argument('--FILTERS', type=int, default=64)
    parser.add_argument('--CROP_SIZE', type=int, default=64)
    # test hyper-parameters
    parser.add_argument('--BATCH_SIZE', type=int, default=1)

    parser.add_argument('--model_path', type=str, default='./models/')
    parser.add_argument('--model_num', type=str, default='500')
    parser.add_argument('--result_path', type=str, default='./test_result/')
    parser.add_argument('--test_dir', type=str, default='')

    config = parser.parse_args()
    config.test_dir = 'a_gre_oled'

    config.model_num = '2000'
    config.name = 'Pediatric_VoluSimu_MOLED_forT2starmapping'


    v_test(config)