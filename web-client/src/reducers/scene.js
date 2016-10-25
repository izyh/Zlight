import { handleActions } from 'redux-actions';
import { combineReducer } from 'redux';
import _ from 'lodash';

module.exports = handleActions({

  // fetch static
  ['scene/fetch/static'](state) {
    return {...state, sSceneLoading: true}
  },
  ['scene/fetch/static.success'](state, action) {
    return {...state, sSceneLoading: false, sSceneList: action.payload}
  },
  ['scene/fetch/static.failure'](state, action) {
    return {...state, sSceneLoading: false, sSceneErr: action.err}
  },

  // modify static
  ['scene/modify/static'](state, action) {
    return {...state, sSceneModifyLoding: true}
  },
  ['scene/modify/static.success'](state, action) {
    const scene = action.payload;
    const newSL = _.chain(state.sSceneList)
      .map(item => {
        if (item.id === scene.id) { return scene }
        else { return item }
      })
      .value();
    return {...state, sSceneModifyLoding: false, sSceneList: newSL}
  },
  ['scene/modify/static.failure'](state, action) {
    return {...state, sSceneModifyLoding: false, sSceneModifyErr: action.err}
  }

}, {
  sSceneList: [],
  sSceneLoading: false,
  sSceneErr: '',
  // modify static
  sSceneModifyLoding: false,
  sSceneModifyErr: ''
});