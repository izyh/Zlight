import { takeLatest, takeEvery } from 'redux-saga';
import { take, call, put, fork, cancel } from 'redux-saga/effects';
import api from '../services/mode';
import actions from '../actions';

function * watchModeSet() {
  yield takeLatest('mode/set', function * (action) {
    try {
      const mode = action.payload;
      const { payload } = yield call(api.setMode, mode);
      yield put(actions['mode/set.success'](payload));
    } catch (e) {
      yield put(actions['notification/error']('模式同步失败', e.toString()));
      yield put(actions['mode/set.failure'](e));
    }
  })
}

function * watchModeFetch() {
  yield takeLatest('mode/fetch', function * (action) {
    try {
      const { payload } = yield call(api.fetchMode);
      yield put(actions['mode/fetch.success'](payload));
    } catch (e) {
      yield put(actions['notification/error']('模式同步失败', e.toString()));
      yield put(actions['mode/fetch.failure'](e));
    }
  })
}

module.exports = function * () {
  yield [
    fork(watchModeFetch),
    fork(watchModeSet),
  ];
  yield put(actions['mode/fetch']())
}