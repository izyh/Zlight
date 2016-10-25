import { takeLatest, takeEvery } from 'redux-saga';
import { take, call, put, fork, cancel } from 'redux-saga/effects';
import { notification } from 'antd';
import actions from '../actions';

function * watchSuccess () {
  yield takeEvery('notification/success', function * (action) {
    const { message, description } = action.payload;
    notification.success({
      message, description
    })
  });
}

function * watchError () {
  yield takeEvery('notification/error', function * (action) {
    const { message, description } = action.payload;
    notification.error({
      message, description
    })
  });
}

module.exports = function * () {
  yield [
    fork(watchSuccess),
    fork(watchError),
  ]
}