import React, {Component, PropTypes} from 'react';
import { connect } from 'react-redux';
import { Link } from 'react-router';
import { Card, Form, Input, Button } from 'antd';
import _ from 'lodash';

@Form.create()
class Meta extends Component {
  constructor(props) {
    super(props);
    this.state = {
      isEditing: false
    }
  }
  handleSubmit(e) {
    this.handleCancel();
    const { name } = this.props.form.getFieldsValue();
    this.props.onChange && this.props.onChange({
      name,
    });
  }
  handleCancel() {
    this.setState({isEditing: false});
  }
  handleEdit() {
    this.setState({isEditing: true});
  }
  render() {
    const { name } = this.props;
    const { isEditing } = this.state;
    const { getFieldProps } = this.props.form;
    const formItemLayout = {
      labelCol: { span: 3 },
      wrapperCol: { span: 14 },
    };
    return (
      <Card title="信息">
        <Form horizontal>
          <Form.Item label="名称" {...formItemLayout}>
            {
              isEditing ?
                <Input type="text" {...getFieldProps("name", {initialValue: name})} /> : 
                <p className="ant-form-text">{name}</p>
            }
          </Form.Item>
        </Form>
        {
          isEditing ?
            <footer style={{textAlign: 'right'}}>
              <Button type="primary" size="small" onClick={this.handleSubmit.bind(this)}>确定</Button>
              <Button type="ghost" size="small" onClick={this.handleCancel.bind(this)}>取消</Button>
            </footer> :
            <footer style={{textAlign: 'right'}}>
              <Button type="ghost" size="small" onClick={this.handleEdit.bind(this)}>编辑</Button>
            </footer>
        }
      </Card>
    )
  }
}

Meta.propTypes = {
  name: PropTypes.string.isRequired,
  onChange: PropTypes.func
}

module.exports = Meta;