import tensorflow as tf

#  [ [ 0  1 ]
#    [0   1 ]
#    [1   0 ] ]

shape = [3, 2]  # tensor形状为3*2
values = [1, 1, 1]  # 含有三个值为1的元素
indices = [[0, 1], [1, 1], [2, 0]]  # tensor位置索引

a = tf.SparseTensor(values=values, indices=indices, dense_shape=shape)
b = tf.sparse_tensor_to_dense(a)

with tf.Session() as sess:
	sess.run(tf.global_variables_initializer())
	res1, res2 = sess.run([a, b])
	print("res1:{}".format(res1))
	print("res2:{}".format(res2))
