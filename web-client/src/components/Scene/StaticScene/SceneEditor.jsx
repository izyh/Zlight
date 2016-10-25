import React, {Component, PropTypes} from 'react';
import { connect } from 'react-redux';
import { Card, Table, Switch, Slider, Button, Modal } from 'antd';
import _ from 'lodash';
import actions from '../../../actions';
import styles from './SceneEditor.module.less';


const tableColumns = [{
  title: '名称',
  dataIndex: 'name',
  key: 'name'
}, {
  title: '调节',
  dataIndex: 'ctrl',
  key: 'ctrl'
}, {
  title: '操作',
  dataIndex: 'option',
  key: 'option'
}];

const appSelectTableCols = [{
  title: '光源',
  dataIndex: 'name',
  key: 'name'
},{
  title: '类型',
  dataIndex: 'type',
  key: 'type'
}, {
  title: '网络地址',
  dataIndex: 'device',
  key: 'device'
}, {
  title: 'MAC地址',
  dataIndex: 'ieee',
  key: 'ieee'
}, {
  title: '端口号',
  dataIndex: 'endPoint',
  key: 'endPoint'
}];


class Editor extends Component {
  constructor(props) {
    super(props);
    this.state = {
      preview: true,
      localSceneItems: _.cloneDeep(props.sceneItems),
      modalOpen: false,
      appSelectedKeys: [],
    }
  }
  componentWillReceiveProps(nextProps) {
    if (this.props.devices == nextProps.devices) {
      // props.sceneItems 变化，则刷新依赖
      this.setState({
        localSceneItems: _.cloneDeep(nextProps.sceneItems)
      })
    }
  }
  getApp(ieee, ep) {
    const { devices } = this.props;    
    const dev = _.find(devices, dev => dev.ieee == ieee);
    if (!dev) return null;
    return _.find(dev.apps, app => app.endPoint == ep);
  }
  handlePreview() {
    const { localSceneItems } = this.state;
    localSceneItems.forEach(item => {
      const { ieee, ep, scenePayload } = item;
      const app = this.getApp(ieee, ep);
      if (app && this.props.onSetAppProps) {
        this.props.onSetAppProps(app.device, app.endPoint, {payload: scenePayload});
      }
    })
  }
  handleSave() {
    this.props.onSave && this.props.onSave(this.state.localSceneItems);
  }
  handleAddApp() {
    this.setState({
      modalOpen: true
    })
  }
  handleAddAppOk() {
    const nl = this.state.localSceneItems.concat();
    const { appSelectedKeys } = this.state;
    const { devices } = this.props;
    _.forEach(appSelectedKeys, key => {
      let [ieee, ep] = key.split('.');
      ep = parseInt(ep);
      const dev = _.find(devices, dev => dev.ieee == ieee);
      const app = _.find(dev.apps, app => app.endPoint == ep);
      const scenePayload = {};
      switch (app.type) {
        case 'lamp': scenePayload.on = false; break;
        case 'gray-lamp': scenePayload.level = 0; break;
      }
      nl.push({ ieee, ep, scenePayload });
    });
    this.setState({
      modalOpen: false,
      localSceneItems: nl
    })
  }
  handleAddAppCancel() {
    this.setState({
      modalOpen: false,
      appSelectedKeys: []
    })
  }
  handleSetAppPayload(originItem, app, payload) {
    this.setState({
      localSceneItems: _.map(this.state.localSceneItems, litem => {
        if (litem.ieee == originItem.ieee && litem.ep == originItem.ep) {
          return Object.assign({}, originItem, { scenePayload: payload })
        } else { return litem }
      })
    })
  }
  handleAppSelectRow(keys) {
    this.setState({
      appSelectedKeys: keys
    })
  }
  items2TableData() {
    const { devices } = this.props;
    const { localSceneItems: sceneItems } = this.state;
    return _.chain(sceneItems)
      .map(item => {
        const { ieee, ep } = item;
        const dev = _.find(devices, dev => dev.ieee == ieee);
        const app = _.find(dev.apps, app => app.endPoint == ep);
        const ctrlNode = (function (self) {
          if (app.type == 'lamp') {
            const hc = (function (item, app, value) {
              this.handleSetAppPayload(item, app, {on: value})
            }).bind(self, item, app);
            return <Switch
              checked={item.scenePayload.on}
              onChange={hc}
            />
          }
          else {
            return <div>暂不支持</div>
          }
        })(this);
        const optNode = <a>删除</a>
        return {
          key: `${ieee}.${ep}`,
          name: app.name,
          ctrl: ctrlNode,
          option: optNode,
          // inject
          originItem: item,
          originDev: dev,
          originApp: app,
        }
      })
      .value();
  }
  apps2TableData() {
    const { devices } = this.props;
    const { localSceneItems: sceneItems } = this.state;
    const apps = [];
    _.forEach(devices, dev => {
      _.forEach(dev.apps, app => {
        if (['lamp', 'grey-lamp'].indexOf(app.type) < 0) return;
        const key = `${dev.ieee}.${app.endPoint}`;
        const isExist = _.some(sceneItems, item => `${item.ieee}.${item.ep}` == key);
        if (isExist) return;
        let typeName = app.type;
        switch (app.type) {
          case 'lamp': typeName = '普通光源'; break;
          case 'grey-lamp': typeName = '可调光源'; break;
        }
        apps.push({
          ...app,
          key,
          type: typeName,
          ieee: dev.ieee,
        })
      })
    });
    return apps;
  }
  render() {
    const { modalOpen, appSelectedKeys } = this.state;
    const appTableRowSelection = {
      selectedRowKeys: appSelectedKeys,
      onChange: this.handleAppSelectRow.bind(this),
    };
    return (
      <Card title="灯光配置">
        <header className={styles.header}>
          <Button onClick={this.handleAddApp.bind(this)}>添加光源</Button>        
          <Button onClick={this.handlePreview.bind(this)}>预览</Button>
          <Button type="primary" onClick={this.handleSave.bind(this)}>保存</Button>
        </header>
        <Table
          columns={tableColumns}
          dataSource={this.items2TableData()}
          pagination={false}
        />
        <Modal
          title="添加光源"
          visible={modalOpen}
          onOk={this.handleAddAppOk.bind(this)}
          onCancel={this.handleAddAppCancel.bind(this)}
        >
          <Table
            columns={appSelectTableCols}
            dataSource={this.apps2TableData()}
            pagination={false}
            rowSelection={appTableRowSelection}
          />
        </Modal>
      </Card>
    )
  }
}

Editor.propTypes = {
  sceneItems: PropTypes.array.isRequired,
  onSave: PropTypes.func,
  // injected
  devices: PropTypes.array.isRequired,
  onSetAppProps: PropTypes.func,
}

module.exports = connect(
  state => ({
    devices: state.device.devices,
  }),
  dispatch => ({
    onSetAppProps: (nwk, ep, props) => {
      dispatch(actions['device/app/prop/set'](nwk, ep, props))
    }
  })
)(Editor);