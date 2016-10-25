import React, {Component, PropTypes} from 'react';
import { Row, Col, Form, Card, Button, Switch, Input } from 'antd';
import Lamp from './Lamp';
import Pulse from './Pulse';
import ListMenu from './ListMenu';
import styles from './ListContainer.module.less';

class ListContainer extends Component {
  constructor(props) {
    super(props);
    this.state = {
      currentIndex: null
    }
    this.handleSelect = this._handleSelect.bind(this);
    this.handleChangeName = this._handleChangeName.bind(this);
    this.handleChangeOnOff = this._handleChangeOnOff.bind(this);
  }
  componentWillReceiveProps (nextProps) {
    if (nextProps.appType != this.props.appType) {
      this.setState({currentIndex: null})
    }
  }
  get current() {
    const { currentIndex } = this.state;
    if (currentIndex === null) {
      return null;
    } else {
      return this.props.apps[currentIndex];
    }
  }
  _handleChangeAppProps(props) {
    const { current } = this;
    if (current) {
      this.props.onChangeAppProps && this.props.onChangeAppProps(
        current.nwk,
        current.endPoint,
        props
      );
    }
  }
  _handleSelect(app, index) {
    this.setState({currentIndex: index});
  }
  _handleChangeName(name) {
    this._handleChangeAppProps({name})
  }
  _handleChangeOnOff(onOff) {
    this._handleChangeAppProps({
      payload: {on: onOff}
    })
  }
  getBody() {
    const { current } = this;
    const { loading } = this.props;
    if (current) {
      switch (current.type) {
        case 'lamp':
          return <Lamp
            name={current.name}
            nwk={current.nwk}
            ieee={current.ieee}
            endPoint={current.endPoint}
            onOff={current.payload.on}
            loading={loading}
            onChangeName={this.handleChangeName}
            onChangeOnOff={this.handleChangeOnOff}
          />
        case 'pulse':
          return <Pulse
            name={current.name}
            nwk={current.nwk}
            ieee={current.ieee}
            endPoint={current.endPoint}
            loading={loading}
            transId={current.payload.transId}
            onChangeName={this.handleChangeName}
          />
        default:
          return <div>{`暂不支持 ${current.type}`}</div>
      }
    } else {
      return <div>请选择一个应用</div>
    }
  }
  render() {
    const { apps, loading } = this.props;
    const { currentIndex } = this.state;
    const listData = apps.map((app, index) => ({
      id: `${app.device}-${app.endPoint}`,
      content: <div title={app.name} className={ index == currentIndex ? styles.listItemActive : styles.listItem }>
        {app.name}
      </div>
    }));
    return (
      <div>
        <Row gutter={16}>
          <Col span={5}>
            <ListMenu data={listData} loading={loading} onSelect={this.handleSelect} />
          </Col>
          <Col span={19}>{this.getBody()}</Col>
        </Row>
      </div>
    )
  }
}

const appType = {
  device: PropTypes.number.isRequired,
  endPoint: PropTypes.number.isRequired,
  type: PropTypes.string.isRequired,
  name: PropTypes.string.isRequired,
  payload: PropTypes.object.isRequired,
  // inject
  nwk: PropTypes.number.isRequired,
  ieee: PropTypes.string.isRequired,
};

ListContainer.propTypes = {
  appType: PropTypes.string.isRequired,
  apps: PropTypes.arrayOf(PropTypes.shape(appType)).isRequired,
  loading: PropTypes.bool.isRequired,
  onChangeAppProps: PropTypes.func
}

module.exports = ListContainer;
