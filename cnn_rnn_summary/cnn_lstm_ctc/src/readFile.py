# -*- coding:utf-8 -*-
import os
import codecs
import numpy as np
train_path = '/home/chenjunxia/tmp/crnn伪代码/cnn_lstm_ctc/data'


train_sample = 'imagenamenew.txt'

filter=[".png"]

arrchar=[]
def readtxt():
	dictpath='../index/char_dict.txt'
	for line2 in open(dictpath):
		# print (line2)
		arrchar.append(line2)
	return arrchar

def all_path(dirname):
	result = []
	arrchar = readtxt()
	print('arrchar=', arrchar)
	for maindir, subdir, file_name_list in os.walk(dirname):
		for filename in file_name_list:
			print('filename=', filename)
			# maindirnw = maindir.split('/')[-1]
			apath = os.path.join(maindir, filename)  # 合并成一个完整路径

			ext = os.path.splitext(apath)[1]  # 获取文件后缀 [0]获取的是除了文件名以外的内容
			print('filename=', filename)
			num_ = filename.count('_')
			if(num_ == 1):
				if '.' in filename:
					dot1 = filename.rindex('_')
					dot2 = filename.rindex('.')

					if (dot2 > int(dot1 + 1)):
						label = filename[int(dot1 + 1):dot2]
						if ' ' in label:
							print('thereis space')
						else:
							label = label.replace("~", "/")
							label = label.replace('?', '？')
							label = label.replace(',', '，')
							label = label.replace('×', '*')
							label = label.replace('！', '!')
							label = label.replace('＞', '>')
							label = label.replace('＜', '<')

							print('label=', label)
							labelnew = []
							for ch in label:
								if ch + '\n' in arrchar:
									labelnew.append(ch)
							relabel = ''.join(labelnew)
							print('labelnew=', relabel)
							if 0< len(relabel) and len(relabel) < 25:
								ff = apath

								result.append(ff)
	return result

if __name__ == '__main__':
	allimg = all_path(train_path)

	print('lenimg=', len(allimg))
	perm = np.arange(len(allimg))
	np.random.shuffle(perm)
	allimg = np.asarray(allimg)
	train_data = allimg[perm]



	with codecs.open(train_sample, 'a+', encoding='utf-8') as f:
		for name in train_data:
			f.write(name)
			f.write('\n')
		f.close()
	print('WRITE SECESSED!')