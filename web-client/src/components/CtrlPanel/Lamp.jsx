import React, {Component, PropTypes} from 'react';
import { Form, Card, Button, Switch, Input } from 'antd';
import styles from './Lamp.module.less';

const Gutter = () => <div className={styles["gutter"]}></div>;

@Form.create()
class Lamp extends Component {
  constructor(props) {
    super(props);
    this.state = {
      isEditing: false
    }
    this.handleEdit = this._handleEdit.bind(this);
    this.handleCancel = this._handleCancel.bind(this);
    this.handleSubmit = this._handleSubmit.bind(this);
    this.handleChangeOnOff = this._handleChangeOnOff.bind(this);
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
  _handleChangeOnOff(onOff) {
    const { onChangeOnOff } = this.props;
    onChangeOnOff && onChangeOnOff(onOff);
  }
  render() {
    const { isEditing } = this.state;
    const { name, nwk, ieee, onOff, loading } = this.props;
    const { getFieldProps } = this.props.form;
    const formItemLayout = {
      labelCol: { span: 3 },
      wrapperCol: { span: 14 },
    };
    return (
      <section>
        <Card title="基本信息">
          <Form horizontal>
            <Form.Item label="灯具名" {...formItemLayout}>
              {
                isEditing ?
                <Input type="text" {...getFieldProps("name", {initialValue: name})} /> :
                <p className="ant-form-text">{name}</p>
              }
            </Form.Item>
            <Form.Item label="网络地址" {...formItemLayout}>
              <p className="ant-form-text">{nwk}</p>
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
          <Switch checked={onOff} onChange={this.handleChangeOnOff} />
        </Card>
      </section>
    )
  }
}
Lamp.propTypes = {
  name: PropTypes.string.isRequired,
  nwk: PropTypes.number.isRequired,
  ieee: PropTypes.string.isRequired,
  endPoint: PropTypes.number.isRequired,
  onOff: PropTypes.bool.isRequired,
  loading: PropTypes.bool.isRequired,
  onChangeName: PropTypes.func,
  onChangeOnOff: PropTypes.func,
}

module.exports = Lamp;
