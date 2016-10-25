import { takeLatest, takeEvery } from 'redux-saga';
import { take, call, put, fork, cancel } from 'redux-saga/effects';
import api from '../services/scene';
import actions from '../actions';

function * watchFetchStaticScene () {
  yield takeLatest('scene/fetch/static', function * (action) {
    try {
      const { payload } = yield call(api.fetchSceneList, 'static');
      yield put(actions['scene/fetch/static.success'](payload));
    } catch (e) {
      yield put(actions['scene/fetch/static.failure'](e));      
    }
  });
}

function * watchModifyStaticScene () {
  yield takeEvery('scene/modify/static', function * (action) {
    try {
      const { payload: scene } = yield call(api.modifyStaticScene, action.payload);
      yield put(actions['scene/modify/static.success'](scene));
    } catch (e) {
      yield put(actions['scene/modify/static.failure'](e));
    }
  })
}

module.exports = function * () {
  yield [
    fork(watchFetchStaticScene),
    fork(watchModifyStaticScene),
  ]
  yield put(actions['scene/fetch/static']());
}