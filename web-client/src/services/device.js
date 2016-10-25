import xFetch from './xFetch';
import qs from 'querystring';

module.exports = {
  /**
   * 获取设备
   * @param  {Object} [query] 查询条件
   * @return {Promise}
   */
  fetchDevices: function (query) {
    return xFetch(`/api/device?${qs.stringify(query)}`);
  },
  fetchDeviceOne: function (nwk) {
    return xFetch(`/api/device/nwk/${nwk}`);
  },
  setAppProps: function (nwk, ep, props) {
    return xFetch(`/api/device/nwk/${nwk}/ep/${ep}`, {
      method: 'PUT',
      body: JSON.stringify(props),
    });
  }
};
