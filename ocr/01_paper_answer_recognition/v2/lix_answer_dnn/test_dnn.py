#!/usr/bin/env python3
# -*- coding:utf-8 -*-
# lix

import torch
import numpy as np
from matplotlib import pyplot as plt
import torchvision.transforms as transforms

import torch.nn.functional as F
from torch.optim import lr_scheduler
import os
import torch.nn as nn
import cv2 

# from PIL import Image

# image = Image.open('test.png')
# transform_img = transforms.Compose([transforms.RandomRotation((-10, 10))])
# image = transform_img(image)  
# image.save("tt1.png")


# image = cv2.imread("test.png", 1).astype(np.uint8)
# # image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
# # image = cv2.resize(image, (32, 32))#.astype(np.float32) # hwc
# transform_img = transforms.Compose([transforms.ToPILImage(), transforms.RandomRotation((-10, 10))]) 
# masks = np.asarray(transform_img(image))  
# cv2.imwrite("tt2.png", masks)

# a = np.random.random((2,4,3))
# transform = transforms.Compose([
#     transforms.ToTensor()
# ])
# b = transform(a)
# print(b.shape)

# exit(0)

# Super parameter ------------------------------------------------------------------------------------
batch_size_train = 8192
batch_size_test = 2048  
batch_size_eval = 1  

learning_rate = 0.001
momentum = 0.5
n_epochs = 50
log_interval = 10  
random_seed = 1024  
torch.manual_seed(random_seed)

os.environ["KMP_DUPLICATE_LIB_OK"] = "TRUE"
torch.backends.cudnn.benchmark = True
device = "cpu" # torch.device("cuda" if torch.cuda.is_available() else "cpu")
torch.cuda.empty_cache()

train_data_path = "svm_data/train/"
test_data_path  = "svm_data/train/"
eval_data_path  = "svm_data/train/"

name_dict = ["v", "x", "o"]

class MyDataset(torch.utils.data.Dataset):
    def __init__(self, file_dir, transform=None):  
        self.transform = transform
        self.data = []
        self.label = []
        for root, _, files in os.walk(file_dir):
            if root == "./": 
                continue
            if len(files) != 0:
                curr_label = int(root.split("/")[-1])
            for filename in files:              
                abspath = os.path.join(root, filename)
                self.data.append(abspath)
                self.label.append(curr_label)

    def __len__(self):
        return len(self.data)
    
    def __getitem__(self, idx):
        image = cv2.imread(self.data[idx]).astype(np.uint8)
        image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        image = cv2.resize(image, (32, 32)).astype(np.float32) # hwc

        if self.transform:
            image = self.transform(image)
        return image, torch.tensor(self.label[idx], dtype=torch.long)


train_dataset = MyDataset(file_dir=train_data_path, 
                          transform=transforms.Compose([
                                   transforms.ToPILImage(),
                                  # torchvision.transforms.RandomAffine(degrees = 0, translate=(0.1, 0.1)),
                                   transforms.RandomRotation((-10, 10)), # rotation（-10,10） 
                                   transforms.ToTensor(),  #  numpy.ndarray to Tensor   hwc2chw /255
                                   ]) )
test_dataset = MyDataset(file_dir=test_data_path,                          
                            transform=transforms.Compose([
                                   transforms.ToPILImage(),
                                   transforms.ToTensor(),  #  numpy.ndarray to Tensor   hwc2chw /255
                                   ]) )
eval_dataset = MyDataset(file_dir=eval_data_path,                          
                            transform=transforms.Compose([
                                   transforms.ToPILImage(),
                                   transforms.ToTensor(),  #  numpy.ndarray to Tensor   hwc2chw /255
                                   ]) )

train_loader = torch.utils.data.DataLoader(dataset=train_dataset,                           
    batch_size=batch_size_train, shuffle=True, num_workers=12, pin_memory=True) 

test_loader = torch.utils.data.DataLoader(dataset=test_dataset, 
    batch_size=batch_size_test, shuffle=False, num_workers=12, pin_memory=True)

eval_loader = torch.utils.data.DataLoader(dataset=eval_dataset, 
    batch_size=batch_size_eval, shuffle=False, num_workers=4, pin_memory=True)

# fig = plt.figure()
# for i in range(12):
#     plt.subplot(3, 4, i+1)
#     plt.tight_layout()
#     image = cv2.imread(train_dataset.data[i]).astype(np.uint8)
#     plt.imshow(image)
#     plt.title("Labels: {}".format(train_dataset.label[i]))
#     plt.xticks([])
#     plt.yticks([])
# plt.show()

class Net(torch.nn.Module):
    def __init__(self):
        super(Net, self).__init__()
        self.num_classes = 3
        # Convolution layer 1 (（w - f + 2 * p）/ s ) + 1             28*28
        self.conv1 = nn.Conv2d(in_channels = 1 , out_channels = 32, kernel_size = 5, stride = 1, padding = 0 )
        self.relu1 = nn.ReLU()
        self.batch1 = nn.BatchNorm2d(32)
        
        # Convolution layer 2                                        24*24
        self.conv2 = nn.Conv2d(in_channels = 32 , out_channels = 32, kernel_size = 5, stride = 1, padding = 0 )
        self.relu2 = nn.ReLU()
        self.batch2 = nn.BatchNorm2d(32)
        self.maxpool1 = nn.MaxPool2d(kernel_size = 2, stride = 2)   # 12*12
        self.conv1_drop = nn.Dropout(0.25)

        # Convolution layer 3                                       # 10*10
        self.conv3 = nn.Conv2d(in_channels = 32, out_channels = 64, kernel_size = 3, stride = 1, padding = 0 )
        self.relu3 = nn.ReLU()
        self.batch3 = nn.BatchNorm2d(64)
        
        # Convolution layer 4                                       # 8*8
        self.conv4 = nn.Conv2d(in_channels = 64, out_channels = 64, kernel_size = 3, stride = 1, padding = 0 )
        self.relu4 = nn.ReLU()
        self.batch4 = nn.BatchNorm2d(64)
        self.maxpool2 = nn.MaxPool2d(kernel_size = 2, stride = 2)   # 128 * 64* 4*4 =  128x1024
        self.conv2_drop = nn.Dropout(0.25)

        # Fully-Connected layer 1        
        self.fc1 = nn.Linear(1024, 256) 
        self.fc1_relu = nn.ReLU()
        self.dp1 = nn.Dropout(0.5)
        
        # Fully-Connected layer 2
        self.fc2 = nn.Linear(256, self.num_classes)

    def forward(self, x):
        # conv layer 1  
        out = self.conv1(x)
        out = self.relu1(out)
        out = self.batch1(out)
        
        # conv layer 2
        out = self.conv2(out)
        out = self.relu2(out)
        out = self.batch2(out)
        
        out = self.maxpool1(out)
        out = self.conv1_drop(out)

        # conv layer 3
        out = self.conv3(out)
        out = self.relu3(out)
        out = self.batch3(out)
        
        # conv layer 4
        out = self.conv4(out)
        out = self.relu4(out)
        out = self.batch4(out)
        
        out = self.maxpool2(out)
        out = self.conv2_drop(out)

        # Flatten 
        out = out.view(out.size(0),-1)

        # FC layer  
        out = self.fc1(out)
        out = self.fc1_relu(out)
        out = self.dp1(out)
        
        # FC 
        out = self.fc2(out)

        return F.log_softmax(out, dim = 1)

def weight_init(m):
    if isinstance(m, torch.nn.Conv2d):
        torch.nn.init.kaiming_normal_(m.weight, mode='fan_out', nonlinearity='relu')
    '''
    elif isinstance(m, nn.Linear):
        nn.init.xavier_normal_(m.weight)
        nn.init.constant_(m.bias, 0)
    elif isinstance(m, nn.BatchNorm2d):
        nn.init.constant_(m.weight, 1)
        nn.init.constant_(m.bias, 0)
    ''' 

train_losses = []
train_counter = []
train_acces = []
test_losses = []
test_counter = [i*len(train_loader.dataset) for i in range(n_epochs + 1)]
test_acces = []

model = Net()
model.to(device)
model.apply(weight_init)

#optimizer = torch.optim.SGD(model.parameters(), lr=learning_rate, momentum=momentum)
optimizer = torch.optim.Adam(model.parameters(), lr=learning_rate)
# optimizer = torch.optim.RMSprop(model.parameters(),lr=learning_rate,alpha=0.99,momentum = momentum)
scheduler = lr_scheduler.ReduceLROnPlateau(optimizer, mode='max', factor=0.5, patience=3, verbose=True, threshold=0.00005, threshold_mode='rel', cooldown=0, min_lr=0, eps=1e-08)

def infer(network=None):
    if network is None:
        network = Net()
        model_path = "./model.pth"
        network.load_state_dict(torch.load(model_path))
        ############################################ or use follow ############
        # model_path = "./model_full.pth"
        # network = torch.load(model_path)
        network.eval()
    network.to(device)

    path = 'svm_data/train/'  
    with torch.no_grad():   
        i = 0; j=0
        for root, _, files in os.walk(path):
            if root == "./": 
                continue
            if len(files) != 0:
                curr_label = int(root.split("/")[-1])

            for filename in files:              
                abspath = os.path.join(root, filename)
                j +=1
                image = cv2.imread(abspath).astype(np.uint8)   
                try:
                    image.shape
                except:
                    print("abspath ", abspath)    
                    continue   

                image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
                image = cv2.resize(image, (32, 32)).astype(np.float32)  ## must convert to fp32 !!!

                transform=transforms.Compose([
                            transforms.ToPILImage(),
                            transforms.ToTensor(),  
                            ])  
                
                image_t = transform(image).view(1, 1, 32, 32)
                output = network(image_t.to(device))

                pred = int(output.data.max(dim = 1, keepdim=True)[1].cpu().item())
                if curr_label != pred:
                    i+=1
                    print(abspath)
                # plt.tight_layout()
                # plt.subplot(121)
                # img = Image.fromarray(image.astype(np.uint8) )
                # plt.imshow(img)
 
                # plt.title("Original Image: {}".format(curr_label))
                # plt.xticks([])
                # plt.yticks([])
                
                # plt.subplot(122)
                # plt.imshow(img, cmap='gray', interpolation='none')
 
                # plt.title("Prediction: {}".format(pred))
                # plt.xlabel("Processed Image")
                # plt.xticks([])
                # plt.yticks([])
                # plt.show()
        print(f">>>>>>>>>>>>>>> bad {i} | total {j} ")            


def infer_v1():
    network = Net()
    model_path = "./model.pth"
    network.load_state_dict(torch.load(model_path, map_location=torch.device(device)))
    ########### or use follow ############
    # model_path = "./model_full.pth"
    # network = torch.load(model_path)
    network.eval()
    # network.to(device)

    path = 'mytest/'  
    with torch.no_grad():   
        i = 0; j=0
        for root, _, files in os.walk(path):
            if root == "./": 
                continue
            if len(files) != 0:
                curr_label = int(root.split("/")[-1])

            for filename in files:              
                abspath = os.path.join(root, filename)
                j +=1
                image = cv2.imread(abspath).astype(np.uint8)   
                try:
                    image.shape
                except:
                    print("abspath ", abspath)    
                    continue   

                image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
                image = cv2.resize(image, (32, 32)).astype(np.float32)  ## must convert to fp32 !!!

                transform=transforms.Compose([
                            transforms.ToPILImage(),
                            transforms.ToTensor(),  
                            ])  
                
                image_t = transform(image).view(1, 1, 32, 32)
                output = network(image_t)
                # output = network(image_t.to(device))

                pred = int(output.data.max(dim = 1, keepdim=True)[1].cpu().item())
                if curr_label != pred:
                    i+=1
                    print(abspath, curr_label, pred)
        print(f">>>>>>>>>>>>>>> bad {i} | total {j} ")            


def train(epoch):
    model.train()
    train_correct = 0

    for batch_idx, data in enumerate(train_loader):
        inputs, target = data

        # reset grad 
        optimizer.zero_grad()

        # forward  
        output = model(inputs.to(device))

        # loss 
        loss = F.nll_loss(output, target.to(device))

        # backward    
        loss.backward()

        # update
        optimizer.step()

        train_pred = output.data.max(dim=1, keepdim=True)[1] # dim = 1 max in each row，[1] index of value，[0] value    

        train_correct += train_pred.eq(target.data.view_as(train_pred).to(device)).sum() # 比较并求正确分类的个数

        print('\r Curr {}th Train Epoch: [{}/{} ({:.0f}%)  ]\tLoss: {:.6f}'.format(
                epoch, (batch_idx + 1) * len(data), len(train_loader.dataset),
                100. * batch_idx / len(train_loader),        loss.item()), end = '', flush=True)

        if batch_idx  % log_interval == 0:
            train_losses.append(loss.item())
            train_counter.append((batch_idx*64) + ((epoch-1)*len(train_loader.dataset)))
    
    train_acc = train_correct / len(train_loader.dataset)
    diff = len(train_loader.dataset) - train_correct
    train_acces.append(train_acc.cpu().numpy().tolist())
    print('\t Curr epoch train Accuracy:{:.2f}%  {}'.format(100. * train_acc, diff))


def test(epoch):
    model.eval() 

    test_loss = 0
    correct = 0
    with torch.no_grad():
        for data, target in test_loader:
            output = model(data.to(device)) 
            #test_loss += F.nll_loss(output, target, size_average=False).item()
            test_loss += F.nll_loss(output, target.to(device), reduction='sum').item()

            pred = output.data.max(dim=1, keepdim=True)[1] # row max index 
            correct += pred.eq(target.data.view_as(pred).to(device)).sum() 

    acc = correct / len(test_loader.dataset) 
    diff = len(test_loader.dataset) - correct
    test_acces.append(acc.cpu().numpy().tolist())
    test_loss /= len(test_loader.dataset)  
    test_losses.append(test_loss)  
    
    # save the best 
    if test_acces[-1] >= max(test_acces):
        print(" save {} ckpt ... ".format(epoch))
        torch.save(model.state_dict(), './model.pth')
        torch.save(optimizer.state_dict(), './optimizer.pth')
        torch.save(model, 'model_full.pth')


    # Test set: Avg. loss: 2.3129, Accuracy: 1205/10000 (12%)
    print('\r Test set \033[1;31m{}\033[0m : Avg. loss: {:.4f}, Accuracy: {}/{}  \033[1;31m({:.2f}%)\033[0m {}, curr_max {:.2f}%\n'\
          .format(epoch, test_loss, correct, len(test_loader.dataset), 100. * acc, diff, 100. * max(test_acces)), end = '') 


if __name__ == '__main__':
    # infer_v1();exit(0)
    # infer(network=None);exit(0)
    test(1)

    ###################################################
    for epoch in range(1, n_epochs + 1):
        scheduler.step(test_acces[-1])
        train(epoch)
        test(epoch)

    print('\n\033[1;31mThe network Max Avg Accuracy : {:.2f}%\033[0m'.format(100. * max(test_acces)))

    fig = plt.figure(figsize=(15, 5)) 
    ax1 = fig.add_subplot(121)

    ax1.plot(train_counter, train_losses, color='blue')
    plt.scatter(test_counter, test_losses, color='red')
    plt.legend(['Train Loss', 'Test Loss'], loc='upper right')
    plt.title('Train & Test Loss')
    plt.xlabel('number of training examples seen')
    plt.ylabel('negative log likelihood loss')

    plt.subplot(122)
    max_test_acces_epoch = test_acces.index(max(test_acces))
    max_test_acces = round(max(test_acces), 4)

    plt.plot([epoch+1 for epoch in range(n_epochs) ], train_acces, color='blue')
    plt.plot([epoch+1 for epoch in range(n_epochs) ], test_acces[1:], color='red')

    plt.plot(max_test_acces_epoch, max_test_acces,'ko') # max value point

    show_max='  ['+str(max_test_acces_epoch ) + ' , '+ str(max_test_acces) + ']'
    plt.annotate(show_max,xy=(max_test_acces_epoch,max_test_acces), 
                xytext=(max_test_acces_epoch,max_test_acces))

    plt.legend(['Train acc', 'Test acc'], loc='lower right')
    plt.title('Train & Test Accuracy')

    plt.xlabel('number of training epoch')
    plt.ylabel('negative log likelihood acc')
    plt.show()


# /home/gwm/anaconda3/envs/torch210/lib/python3.8/site-packages/torch/nn/modules/conv.py:456: UserWarning: Applied workaround for CuDNN issue, install nvrtc.so (Triggered internally at ../aten/src/ATen/native/cudnn/Conv_v8.cpp:80.)
#   return F.conv2d(input, weight, bias, self.stride,

