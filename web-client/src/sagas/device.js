import { takeLatest, takeEvery } from 'redux-saga';
import { take, call, put, fork, cancel } from 'redux-saga/effects';
import api from '../services/device';
import actions from '../actions/device';

function * watchFetchDevice() {
  yield takeLatest('device/fetch/all', function * () {
    try {
      const { type, payload } = yield call(api.fetchDevices);
      yield put(actions['device/fetch/all.success'](payload));
    } catch (e) {
      yield put(actions['device/fetch/all.failure'](e));
    }
  });
}

function * watchFetchDeviceOne() {
  yield takeEvery('device/fetch/one', function * (action) {
    try {
      const { payload: nwk } = action;
      const { type, payload: device } = yield call(api.fetchDeviceOne, nwk);
      yield put(actions['device/fetch/one.success'](device));
    } catch (e) {
      yield put(actions['device/fetch/one.failure'](e));
    }
  });
}

function * watchAppSet() {
  yield takeEvery('device/app/prop/set', function * (action) {
    const { nwk, ep, props } = action.payload;
    try {
      const { type, payload: app } = yield call(api.setAppProps, nwk, ep, props);
      yield put(actions['device/app/prop/set.success'](app));
    } catch (e) {
      yield put(actions['device/app/prop/set.failure'](e));
    }
  })
}

module.exports = function * () {
  yield [
    fork(watchFetchDevice),
    fork(watchFetchDeviceOne),
    fork(watchAppSet),
  ],
  yield put(actions['device/fetch/all']());
};
