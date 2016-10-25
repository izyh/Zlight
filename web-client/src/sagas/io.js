import { takeLatest, takeEvery } from 'redux-saga';
import { take, call, put, fork, cancel } from 'redux-saga/effects';
import actions from '../actions/io';
import deviceActions from '../actions/device';


function * watchConnect() {
  yield takeLatest('io/reconnect.success', function * (action) {
    // 重连触发刷新设备列表
    yield put(deviceActions['device/fetch/all']());
  })
}

function * watchReceive() {
  yield takeLatest('io/receive.success', function * (action) {
    const { event, data } = action.payload;
    if (event == 'device/join.success') {
      yield put(deviceActions['device/fetch/all']());
    } else if (event == 'device/change.success') {
      const { nwk, ep } = data;
      yield put(deviceActions['device/fetch/one'](nwk));
    }
  })
}

module.exports = function * () {
  yield [
    fork(watchConnect),
    fork(watchReceive),
  ];
};
