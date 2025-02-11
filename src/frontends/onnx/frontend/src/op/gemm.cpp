// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "op/gemm.hpp"

#include "openvino/op/add.hpp"
#include "openvino/op/constant.hpp"
#include "openvino/op/matmul.hpp"
#include "openvino/op/multiply.hpp"
#include "ov_models/ov_builders/reshape.hpp"

using namespace ov::op;

OPENVINO_SUPPRESS_DEPRECATED_START
namespace ngraph {
namespace onnx_import {
namespace op {
namespace set_1 {
OutputVector gemm(const Node& node) {
    OutputVector inputs{node.get_ng_inputs()};
    Output<ov::Node> input_a = inputs.at(0);
    Output<ov::Node> input_b = inputs.at(1);
    Output<ov::Node> input_c;

    if (inputs.size() == 3) {
        input_c = inputs.at(2);
    } else {
        input_c = v0::Constant::create(input_b.get_element_type(), ov::Shape{}, {0});
    }

    const auto alpha = node.get_attribute_value<float>("alpha", 1);
    const auto beta_node = node.get_attribute_as_constant<float>("beta", 1, input_c.get_element_type());

    const bool trans_a = node.get_attribute_value<int64_t>("transA", 0);
    const bool trans_b = node.get_attribute_value<int64_t>("transB", 0);

    if (trans_a) {
        input_a = ov::op::util::transpose(input_a);
    }

    if (trans_b) {
        input_b = ov::op::util::transpose(input_b);
    }

    input_a = ov::op::util::flatten(input_a, 1);
    input_b = ov::op::util::flatten(input_b, 1);

    std::shared_ptr<ov::Node> matmul_node = std::make_shared<v0::MatMul>(input_a, input_b);

    if (alpha != 1) {
        const auto alpha_node = v0::Constant::create(input_b.get_element_type(), Shape{}, {alpha});
        matmul_node = std::make_shared<v1::Multiply>(matmul_node, alpha_node);
    }

    auto beta_times_input_c = std::make_shared<v1::Multiply>(beta_node, input_c);

    return OutputVector{std::make_shared<v1::Add>(matmul_node, beta_times_input_c)};
}

}  // namespace set_1

namespace set_6 {
OutputVector gemm(const Node& node) {
    OutputVector inputs{node.get_ng_inputs()};
    Output<ov::Node> input_a = inputs.at(0);
    Output<ov::Node> input_b = inputs.at(1);
    Output<ov::Node> input_c;

    if (inputs.size() == 3) {
        input_c = inputs.at(2);
    } else {
        input_c = v0::Constant::create(input_b.get_element_type(), ov::Shape{}, {0});
    }

    const auto alpha_node = node.get_attribute_as_constant<float>("alpha", 1, input_b.get_element_type());
    const auto beta_node = node.get_attribute_as_constant<float>("beta", 1, input_c.get_element_type());

    const bool trans_a = node.get_attribute_value<int64_t>("transA", 0);
    const bool trans_b = node.get_attribute_value<int64_t>("transB", 0);

    const auto matmul_node = std::make_shared<v0::MatMul>(input_a, input_b, trans_a, trans_b);
    const auto matmul_times_alpha = std::make_shared<v1::Multiply>(matmul_node, alpha_node);

    const auto beta_times_input_c = std::make_shared<v1::Multiply>(beta_node, input_c);
    const std::string onnx_name = !node.get_name().empty() ? node.get_name() : node.output(0);
    matmul_node->set_friendly_name(onnx_name + "/WithoutBiases");
    return {std::make_shared<v1::Add>(matmul_times_alpha, beta_times_input_c)};
}

}  // namespace set_6

}  // namespace op

}  // namespace  onnx_import

}  // namespace  ngraph
