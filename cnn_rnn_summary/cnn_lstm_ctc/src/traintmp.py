import os
os.environ["CUDA_VISIBLE_DEVICES"] = "0"
import sys
import time
import numpy as np
import tensorflow as tf
import xlrd
import config
import cv2
import gc
import codecs
from model import CRNN
# label_to_array
from utils import resize_image,label_to_array,sparse_tuple_from
import json
import random
iteration_count = 10
batch_image =32
batch_size = 16

def load_data_big(folders):
    examples = []
    count = 0
    for folder in folders:

        if ('.png' in folder) and ('/' in folder):
            filepath,filename = os.path.split(folder)
            count += 1

            arr,initial_len = resize_image(folder)
            num_ = filename.count('_')
            if (num_ == 1):
                dot1 = filename.rindex('_')
                dot2 = filename.rindex('.')

                if (dot2 > int(dot1 + 1)):
                    tmp = filename[int(dot1 + 1):dot2]
                    tmp = tmp.replace("~", "/")
                    tmp = tmp.replace('?', '？')
                    tmp = tmp.replace(',', '，')
                    tmp = tmp.replace('×', '*')
                    tmp = tmp.replace('！', '!')
                    tmp = tmp.replace('＞', '>')
                    tmp = tmp.replace('＜', '<')
                    examples.append(
                        (
                            arr,
                            tmp,
                            initial_len
                        )
                    )
    # print('exampleshape=',len(examples))
    return examples
def loaddict():
    xlsxpath = '../index/words_index.xlsx'
    file = xlrd.open_workbook(xlsxpath)
    try:
        sh = file.sheet_by_name(u"Sheet1")
    except:
        print("no sheet")
    dict = {}
    for i in range(0, 6866):
        char = sh.cell_value(i, 0)
        if type(char) == type(1.0):
            char = int(char)
        index = int(sh.cell(i, 1).value)
        dict[char] = index - 1
    return dict
if __name__ == '__main__':
	data_dict = loaddict()

	model_save_dir = '../model_tmp'
	restore = False
	imagefiles = []
	with codecs.open("imagenamenew.txt", 'r', encoding='utf-8') as file:
		lines = file.readlines()
		for line in lines:
			if line != '\n':
				# line = line.strip("\n")
				imagefiles.append(line.strip())


	inputs = tf.placeholder(tf.float32, [batch_size, 32, None, 1], name='inputs')
	# The CRNN
	crnn = CRNN(inputs)
	# Our target output
	targets = tf.sparse_placeholder(tf.int32, name='targets')
	# The length of the sequence
	seq_len = tf.placeholder(tf.int32, [None], name='seq_len')

	logits = tf.reshape(crnn, [-1, 512])  # (batchsize x 37) x 512

	W = tf.Variable(tf.truncated_normal([512, config.NUM_CLASSES], stddev=0.1, dtype=tf.float32), name="W")
	b = tf.Variable(tf.constant(0., shape=[config.NUM_CLASSES], dtype=tf.float32), name="b")
	print(logits.get_shape())

	logits = tf.matmul(logits, W) + b
	print(logits.get_shape())
	logits = tf.reshape(logits, [batch_size, -1, config.NUM_CLASSES])  # batch_size x 36
	print(logits.get_shape())
	# Final layer, the output of the BLSTM
	logits = tf.transpose(logits, (1, 0, 2), name="final_logits")
	# 36 x batch_size x NUM_CLASSES
	global_step = tf.Variable(0, trainable=False)
	loss = tf.nn.ctc_loss(targets, logits, seq_len)
	cost = tf.reduce_mean(loss)
	optimizer = tf.train.AdadeltaOptimizer(learning_rate=0.1).minimize(loss=cost, global_step=global_step)
	# The decoded answer
	decoded, log_prob = tf.nn.ctc_beam_search_decoder(logits, seq_len)
	# The error rate
	seq_dis = tf.reduce_mean(tf.edit_distance(tf.cast(decoded[0], tf.int32), targets))
	# variables = tf.trainable_variables()
	saver = tf.train.Saver()

	sess_config = tf.ConfigProto()
	sess_config.gpu_options.per_process_gpu_memory_fraction = 0.8
	sess_config.gpu_options.allow_growth = True

	sess = tf.Session(config=sess_config)
	weights_path = '/home/eai/code/crnn-code/model-backup/gray/model_jd0703/model.ckpt'
	with sess.as_default():
		if restore:
			print('restore from model!!!!')
			saver.restore(sess=sess, save_path=weights_path)
		else:
			init = tf.global_variables_initializer()
			sess.run(init)
		print('============begin training=============')

		for it in range(0, iteration_count):
			i = 0
			print('epoch:{}/1000'.format(it))
			random.shuffle(imagefiles)
			# print('imgfiles=',imagefiles)
			for iter in range((len(imagefiles) // batch_image)):
				print('iter=',iter)
				imagepath = imagefiles[iter * batch_image:(iter + 1) * batch_image]
				# print('imagepath=',imagepath)
				# train_data = load_data_big(imagepath)
				train_data = load_data_big(imagepath)
				# print('traindata=',train_data)

				for b in [train_data[x * batch_size:x * batch_size + batch_size] for x in
						  range(0, int(len(train_data) / batch_size))]:
					in_data, labels, data_seq_len = zip(*b)
					for lbl in labels:

						lbarr=label_to_array(lbl, data_dict)

					data_targets = np.asarray([label_to_array(lbl, data_dict) for lbl in labels])

					data_targets = sparse_tuple_from(data_targets)

					data_shape = np.shape(in_data)

					in_data = np.reshape(in_data, (data_shape[0], data_shape[1], data_shape[2], 1))
					costacc, _ = sess.run(

						[cost, optimizer],
						{
							inputs: in_data,
							targets: data_targets,
							seq_len: data_seq_len
						}
					)

			i += 1
			print('epoch:{}/1000,cost={},iter={}'.format(it, costacc, i))
			# del train_data
			# gc.collect()

			# tf.reset_default_graph()  # 重置默认图
			if (it % 2 == 0):
				checkpoint_path = os.path.join(model_save_dir, 'model.ckpt')
				saver.save(sess, checkpoint_path)
			print('epoch training end!!!!!')
