// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "op/onehot.hpp"

#include "openvino/op/constant.hpp"
#include "openvino/op/convert.hpp"
#include "openvino/op/one_hot.hpp"
#include "openvino/op/split.hpp"
#include "utils/reshape.hpp"

using namespace ov::op;

OPENVINO_SUPPRESS_DEPRECATED_START
namespace ngraph {
namespace onnx_import {
namespace op {
namespace set_1 {
OutputVector onehot(const Node& node) {
    OutputVector inputs{node.get_ng_inputs()};
    auto indices = std::make_shared<v0::Convert>(inputs.at(0), element::i64);
    auto depth = std::make_shared<v0::Convert>(reshape::interpret_as_scalar(inputs.at(1)), element::i64);
    // Rank 1 tensor containing exactly two elements: [off_value, on_value]
    auto values = inputs.at(2);
    auto split_axis = v0::Constant::create(element::i64, {}, {0});
    auto off_on_values = std::make_shared<v1::Split>(values, split_axis, 2);
    auto off_value = reshape::interpret_as_scalar(off_on_values->output(0));
    auto on_value = reshape::interpret_as_scalar(off_on_values->output(1));

    auto axis = node.get_attribute_value<std::int64_t>("axis", -1);

    return {std::make_shared<v1::OneHot>(indices, depth, on_value, off_value, axis)};
}

}  // namespace set_1

}  // namespace op

}  // namespace  onnx_import

}  // namespace  ngraph
