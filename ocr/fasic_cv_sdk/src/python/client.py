#!/usr/bin/env python
import sys
sys.path.append('../../thrift/gen-py')
import argparse
import time
import threading

from cv_common import VisionServices
from cv_types.ttypes import *

from thrift.transport import TSocket, TTransport
from thrift.protocol import TBinaryProtocol

import itertools
import multiprocessing as mp
result_queue = mp.Queue()


class ThriftProcessor(threading.local):
    def __init__(self, ip=None, port=None):
        self.host, self.port = ip, port
        print('self.host, self.port is', self.host, self.port)
        self.transport = TSocket.TSocket(self.host, self.port)
        self.transport.setTimeout(1000000)
        self.transport = TTransport.TBufferedTransport(self.transport)
        self.protocol = TBinaryProtocol.TBinaryProtocol(self.transport)
        self.client = VisionServices.Client(self.protocol)
        self.transport.open()

    def __del__(self):
        self.transport.close()

    def process(self, req):
        rsp = self.client.GetModelVersion(req)
        return rsp


def get_args(model_status, ip, port):
    return model_status, ip, port


def query(status):
    model_status, ip, port = get_args(*status)
    global result_queue
    # 道玄：build up connection
    try:
        if args.run_local:
            client = ThriftProcessor(ip=ip, port=port)
        else:
            print('TODO: implement bilibili based thrift socket...')
    except Exception as e:
        print(e)
    req = ModelVersionReq()
    req.status = str(model_status)
    try:
        # 道玄: call service
        rsp = client.process(req)
        print(rsp.model_version.model_version)
        result_queue.put("%s\n" % rsp.model_version.model_version)
    except Exception as e:
        print(e)
    return


def write_to_file(result_path):
    global result_queue
    result = open(result_path, "w")
    while True:
        line = result_queue.get()
        if line != None:
            result.write(line)
        else:
            break
    result.close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--test_num', default=20, type=int)
    parser.add_argument('--run_local', default=1, type=int)
    parser.add_argument('--run_par', default=0, type=int)
    parser.add_argument('--ip', required=True)
    parser.add_argument('--port', required=True)

    args = parser.parse_args()

    output_log = '../../service_log/client_log.txt'
    log_out = open(output_log, 'a')

    time_cost = time.time()
    if args.run_par is 0:
        # 道玄：build up connection
        try:
            if args.run_local:
                client = ThriftProcessor(ip=args.ip, port=args.port)
            else:
                print('TODO: implement bilibili based thrift socket...')
        except Exception as e:
            print(e)
        for i in range(0, args.test_num):
            req = ModelVersionReq()
            req.status = str(i)
            try:
                # 道玄: call service
                rsp = client.process(req)
                print(rsp.model_version.model_version)
                log_out.write('%s\n' % rsp.model_version.model_version)
            except Exception as e:
                print(e)
    else:
        # 道玄：connection is built within every single thread
        data = zip(range(0, args.test_num), itertools.repeat(args.ip), itertools.repeat(args.port))
        write_p = mp.Process(target=write_to_file, args=(output_log,))
        write_p.start()
        p = mp.Pool(10)
        for one in data:
            p.apply_async(func=query, args=(one,))
        p.close()
        p.join()
        p.terminate()
        result_queue.put(None)
        write_p.join()
    time_cost = time.time() - time_cost
    print('time cost is:', time_cost)
    log_out.close()
