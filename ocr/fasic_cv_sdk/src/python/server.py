#!/usr/bin/env python
import sys
sys.path.append('../../thrift/gen-py')

import argparse
import time
import random

from cv_common import VisionServices
from cv_types.ttypes import *

from thrift.transport import TSocket, TTransport
from thrift.protocol import TBinaryProtocol
from thrift.server import TServer

import os
import shutil
from logger import get_logger


def get_args():
    parser = argparse.ArgumentParser(description='Face Recognition Server')
    parser.add_argument('--port', type=int, default=9416, help='Server Port, default = 9416')
    parser.add_argument('--worker', type=int, default=1, help='Process worker number')
    args = parser.parse_args()
    return args.port, args.worker


class VisionServicesHandler:
    def __init__(self, info_logger=None, error_logger=None):
        self.info_logger = info_logger
        self.error_logger = error_logger
        return

    def __del__(self):
        return

    def GetModelVersion(self, req):
        model_id = req.status
        rsp = ModelVersionRsp()
        rsp.model_version = ModelVersion()
        rsp.model_version.model_version = '0.0.' + model_id + ' for test!'
        print(rsp.model_version.model_version)

        # 道玄: this is for multi threads testing
        wait_time = random.random()  # [0,1)
        time.sleep(wait_time)

        return rsp


if __name__ == '__main__':
    # 道玄: logger
    if os.path.exists('../../service_log'):
        shutil.rmtree('../../service_log')
    os.makedirs('../../service_log/info_log')
    os.makedirs('../../service_log/error_log')
    info_logger = get_logger(
        name=__name__ + '_info', filename='../../service_log/info_log/info.log', level='info')
    error_logger = get_logger(
        name=__name__ + '_error',
        filename='../../service_log/error_log/error.log',
        level='error')

    # 道玄：定义服务
    handler = VisionServicesHandler(info_logger=info_logger, error_logger=error_logger)
    processor = VisionServices.Processor(handler)

    port, worker = get_args()
    print('port is:', port)
    print('worker number is:', worker)
    transport = TSocket.TServerSocket(port=port)
    tfactory = TTransport.TBufferedTransportFactory()
    pfactory = TBinaryProtocol.TBinaryProtocolFactory(strictRead=True, strictWrite=False)
    server = TServer.TThreadedServer(processor, transport, tfactory, pfactory)
    print('starting thrift server in python...')
    server.serve()
    print('done!')
