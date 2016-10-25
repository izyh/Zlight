import xFetch from './xFetch';
import qs from 'querystring';

module.exports = {
  /**
   * 获取场景列表
   * @params {String} type - static / auto
   */
  fetchSceneList (type) {
    return xFetch(`/api/scene/store/${type}`)
  },

  modifyStaticScene (scene) {
    return xFetch(`/api/scene/store/static/id/${scene.id}`, {
      method: 'PUT',
      body: JSON.stringify(scene)
    })
  }
};
