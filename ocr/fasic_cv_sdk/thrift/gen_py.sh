#!/usr/bin/env bash

thrift -gen py base.thrift
thrift -gen py cv_types.thrift
thrift -gen py cv_common.thrift
