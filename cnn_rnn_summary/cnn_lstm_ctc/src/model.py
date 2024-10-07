from __future__ import print_function
import tensorflow as tf

from tensorflow.contrib import rnn
import numpy as np
import config

def BidirectionnalRNN(inputs):
    """
        Bidirectionnal LSTM Recurrent Neural Network part
    """

    # Forward
    lstm_fw_cell = rnn.BasicLSTMCell(256, forget_bias=1.0)
    # Backward
    lstm_bw_cell = rnn.BasicLSTMCell(256, forget_bias=1.0)

    outputs, _, _ = rnn.static_bidirectional_rnn(lstm_fw_cell, lstm_bw_cell, inputs, dtype=tf.float32)

    return outputs

def CNN(inputs):
    """
        Convolutionnal Neural Network part
    """
    #input: N x 32 x 160 x 3
    # 64 / 3 x 3 / 1 / 1
    conv1 = tf.layers.conv2d(inputs = inputs, filters = 64, kernel_size = (3, 3), padding = "same", activation=tf.nn.relu) # N x 32 x160 x 64

    # 2 x 2 / 1
    pool1 = tf.layers.max_pooling2d(inputs=conv1, pool_size=[2, 2], strides=2)                                              # N x 16 x 80 x 64

    # 128 / 3 x 3 / 1 / 1
    conv2 = tf.layers.conv2d(inputs = pool1, filters = 128, kernel_size = (3, 3), padding = "same", activation=tf.nn.relu)  #  N x 16 x 80 x 128

    # 2 x 2 / 1
    pool2 = tf.layers.max_pooling2d(inputs=conv2, pool_size=[2, 2], strides=[2,1])                                               # N x 8 x 80 x 128

    # 256 / 3 x 3 / 1 / 1
    conv3 = tf.layers.conv2d(inputs = pool2, filters = 256, kernel_size = (3, 3), padding = "same", activation=tf.nn.relu)   # N x 8 x 80 x 256

    # 256 / 3 x 3 / 1 / 1
    conv4 = tf.layers.conv2d(inputs = conv3, filters = 256, kernel_size = (3, 3), padding = "same", activation=tf.nn.relu)   # N x 8 x 80 x 256

    # 1 x 2 / 1
    pool3 = tf.layers.max_pooling2d(inputs=conv4, pool_size=[1, 2], strides=2)                                                 # N x 4 x 40 x 256

    # 512 / 3 x 3 / 1 / 1
    conv5 = tf.layers.conv2d(inputs = pool3, filters = 512, kernel_size = (3, 3), padding = "same", activation=tf.nn.relu)    # N x 4 x 40 x 512

    # Batch normalization layer
    bnorm1 = tf.layers.batch_normalization(conv5)                                                                              # N x 4 x 40 x 512

    # 512 / 2 x 2 / 1 / 1
    conv6 = tf.layers.conv2d(inputs = bnorm1, filters = 512, kernel_size = (2, 2), padding = "same", activation=tf.nn.relu)   # N x 4 x 40 x 512

    #Batch normalization layer
    bnorm2 = tf.layers.batch_normalization(conv6)                                                                               # N x 4 x 40 x 512

    # 1 x 2 / 2
    pool4 = tf.layers.max_pooling2d(inputs=conv6, pool_size=[1, 2], strides=[2,1])                                                  # N x 2 x 40 x 512

    # 512 / 2 x 2 / 1 / 0
    conv7 = tf.layers.conv2d(inputs = pool4, filters = 512, kernel_size = (2, 2), padding = "valid", activation=tf.nn.relu,name="conv7") # N x 1 x 37 x 512

    return conv7

def MapToSequences(x):
    x = tf.squeeze(x, [1])  #N x 36 x 512
    x = tf.unstack(x)       #相单于变成一个样本变成一行
    return x

def CRNN(x):
    return BidirectionnalRNN(MapToSequences(CNN(x)))

if __name__ == '__main__':
    batch_size = 32

    inputs = tf.placeholder(tf.float32, [batch_size, 32, 160, 1], name='inputs')

    crnn = CRNN(inputs)
    print('crnn=',crnn)
    print('crnn222!!!!!')
    targets = tf.sparse_placeholder(tf.int32, name='targets')
    # The length of the sequence
    seq_len = tf.placeholder(tf.int32, [None], name='seq_len')
    logits = tf.reshape(crnn, [-1, 512])  # (batchsize x 37) x 512

    W = tf.Variable(tf.truncated_normal([512, config.NUM_CLASSES], stddev=0.1, dtype=tf.float32), name="W")
    b = tf.Variable(tf.constant(0., shape=[config.NUM_CLASSES], dtype=tf.float32), name="b")
    print(logits.get_shape())

    logits = tf.matmul(logits, W) + b
    print(logits.get_shape())
    logits = tf.reshape(logits, [batch_size, -1, config.NUM_CLASSES])  # batch_size x 36 x NUM_CLASSES
    print(logits.get_shape())
    # Final layer, the output of the BLSTM
    logits = tf.transpose(logits, (1, 0, 2), name="final_logits")
    print('endlogits=',logits)
    loss = tf.nn.ctc_loss(targets, logits, seq_len)
    cost = tf.reduce_mean(loss)



























