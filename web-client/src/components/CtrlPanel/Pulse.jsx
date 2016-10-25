import React, {Component, PropTypes} from 'react';
import { Form, Card, Button, Switch, Input } from 'antd';
import styles from './Pulse.module.less';

const Gutter = () => <div className={styles["gutter"]}></div>;

@Form.create()
class Pulse extends Component {
  constructor(props) {
    super(props);
    this.state = {
      isEditing: false
    }
    this.handleEdit = this._handleEdit.bind(this);
    this.handleCancel = this._handleCancel.bind(this);
    this.handleSubmit = this._handleSubmit.bind(this);
  }
  _handleCancel() {
    this.setState({isEditing: false});
  }
  _handleEdit() {
    this.setState({isEditing: true});
  }
  _handleSubmit() {
    const { name } = this.props.form.getFieldsValue();
    const { onChangeName } = this.props;
    this.handleCancel();
    onChangeName && onChangeName(name);
  }
  render() {
    const { isEditing } = this.state;
    const { name, nwk, ieee, endPoint, onOff, loading, transId } = this.props;
    const { getFieldProps } = this.props.form;
    const formItemLayout = {
      labelCol: { span: 3 },
      wrapperCol: { span: 14 },
    };
    return (
      <section>
        <Card title="基本信息">
          <Form horizontal>
            <Form.Item label="轻触开关名" {...formItemLayout}>
              {
                isEditing ?
                <Input type="text" {...getFieldProps("name", {initialValue: name})} /> :
                <p className="ant-form-text">{name}</p>
              }
            </Form.Item>
            <Form.Item label="网络地址" {...formItemLayout}>
              <p className="ant-form-text">{nwk}</p>
            </Form.Item>
            <Form.Item label="端口号" {...formItemLayout}>
              <p className="ant-form-text">{endPoint}</p>
            </Form.Item>
            <Form.Item label="MAC" {...formItemLayout}>
              <p className="ant-form-text">{ieee}</p>
            </Form.Item>
          </Form>
          {
            isEditing ?
            <footer style={{textAlign: 'right'}}>
              <Button type="primary" size="small" onClick={this.handleSubmit}>确定</Button>
              <Button type="ghost" size="small" onClick={this.handleCancel}>取消</Button>
            </footer> :
            <footer style={{textAlign: 'right'}}>
              <Button type="ghost" size="small" onClick={this.handleEdit}>编辑</Button>
            </footer>
          }
        </Card>
        <Gutter />
        <Card title="控制器">
          <p>触发ID：{transId}</p>
        </Card>
        <Gutter />
        <Card title="设备绑定">
          <p>暂无</p>
        </Card>
      </section>
    )
  }
}
Pulse.propTypes = {
  name: PropTypes.string.isRequired,
  nwk: PropTypes.number.isRequired,
  ieee: PropTypes.string.isRequired,
  endPoint: PropTypes.number.isRequired,
  loading: PropTypes.bool.isRequired,
  transId: PropTypes.number.isRequired,
  onChangeName: PropTypes.func,
}

module.exports = Pulse;
