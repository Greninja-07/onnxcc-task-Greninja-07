#!/usr/bin/env python3
"""
Test model fixture generator for onnxcc.

Generates a tiny 4 -> 8 -> 2 Multi-Layer Perceptron (MLP) with ReLU activation
after each layer at ONNX opset 13, along with a deterministic raw float32 input
tensor of shape (1, 4) serialized as 16 raw bytes.

Graph Specification:
  - Input: 'X' with shape [1, 4], float32
  - Layer 1:
      MatMul(X, W1) -> shape [1, 8]
      Add(matmul_1, B1) -> shape [1, 8]
      Relu(add_1) -> shape [1, 8]
  - Layer 2:
      MatMul(relu_1, W2) -> shape [1, 2]
      Add(matmul_2, B2) -> shape [1, 2]
      Relu(add_2) -> shape [1, 2]
  - Output: 'Y' with shape [1, 2], float32

Counts:
  - Node count: 6 (2 MatMul, 2 Add, 2 Relu)
  - Initializer count: 4 (W1: [4, 8], B1: [1, 8], W2: [8, 2], B2: [1, 2])
"""

import argparse
from pathlib import Path
import numpy as np
import onnx
from onnx import helper, TensorProto, numpy_helper

# Fixed seed for idempotency and deterministic fixture generation
RANDOM_SEED = 42

def build_mlp_model() -> onnx.ModelProto:
    rng = np.random.RandomState(RANDOM_SEED)

    # Weights and biases for 4 -> 8 -> 2 MLP
    w1_val = rng.randn(4, 8).astype(np.float32)
    b1_val = rng.randn(1, 8).astype(np.float32)
    w2_val = rng.randn(8, 2).astype(np.float32)
    b2_val = rng.randn(1, 2).astype(np.float32)

    # Initializers (Total: 4)
    w1_init = numpy_helper.from_array(w1_val, name="W1")
    b1_init = numpy_helper.from_array(b1_val, name="B1")
    w2_init = numpy_helper.from_array(w2_val, name="W2")
    b2_init = numpy_helper.from_array(b2_val, name="B2")

    # Graph Inputs and Outputs
    x_input = helper.make_tensor_value_info("X", TensorProto.FLOAT, [1, 4])
    y_output = helper.make_tensor_value_info("Y", TensorProto.FLOAT, [1, 2])

    # Nodes (Total: 6, using only MatMul, Add, and Relu)
    # Layer 1
    node_matmul1 = helper.make_node("MatMul", inputs=["X", "W1"], outputs=["matmul1_out"], name="MatMul_1")
    node_add1 = helper.make_node("Add", inputs=["matmul1_out", "B1"], outputs=["add1_out"], name="Add_1")
    node_relu1 = helper.make_node("Relu", inputs=["add1_out"], outputs=["relu1_out"], name="Relu_1")

    # Layer 2
    node_matmul2 = helper.make_node("MatMul", inputs=["relu1_out", "W2"], outputs=["matmul2_out"], name="MatMul_2")
    node_add2 = helper.make_node("Add", inputs=["matmul2_out", "B2"], outputs=["add2_out"], name="Add_2")
    node_relu2 = helper.make_node("Relu", inputs=["add2_out"], outputs=["Y"], name="Relu_2")

    nodes = [
        node_matmul1,
        node_add1,
        node_relu1,
        node_matmul2,
        node_add2,
        node_relu2,
    ]

    initializers = [w1_init, b1_init, w2_init, b2_init]

    # Graph construction
    graph_def = helper.make_graph(
        nodes=nodes,
        name="TinyMLP_4_8_2",
        inputs=[x_input],
        outputs=[y_output],
        initializer=initializers,
    )

    # Model construction at opset 13
    opset_import = [helper.make_opsetid("", 13)]
    model_def = helper.make_model(
        graph_def,
        producer_name="onnxcc-fixture-generator",
        opset_imports=opset_import,
    )

    return model_def

def generate_test_input() -> np.ndarray:
    rng = np.random.RandomState(RANDOM_SEED + 1)
    input_data = rng.randn(1, 4).astype(np.float32)
    return input_data

def main():
    parser = argparse.ArgumentParser(description="Generate ONNX test model fixtures for onnxcc.")
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=Path(__file__).resolve().parent.parent / "tests" / "fixtures",
        help="Directory to write generated fixture files (default: tests/fixtures)",
    )
    args = parser.parse_args()

    args.output_dir.mkdir(parents=True, exist_ok=True)
    model_path = args.output_dir / "mlp.onnx"
    input_bin_path = args.output_dir / "mlp_input.bin"

    print(f"Generating MLP model at opset 13...")
    model = build_mlp_model()

    # Verify model with onnx.checker
    onnx.checker.check_model(model)
    print("onnx.checker validation: PASSED")

    # Print node and initializer counts and op types
    op_types = [node.op_type for node in model.graph.node]
    print(f"Node count: {len(model.graph.node)} (Expected: 6)")
    print(f"Initializer count: {len(model.graph.initializer)} (Expected: 4)")
    print(f"Op types in graph: {op_types}")

    # Ensure ONLY MatMul, Add, and Relu are used
    allowed_ops = {"MatMul", "Add", "Relu"}
    for op in op_types:
        assert op in allowed_ops, f"Unexpected op type: {op}. Only {allowed_ops} are allowed."
    print("Operator check: All ops are strictly MatMul, Add, or Relu (no Gemm).")

    # Save ONNX model
    onnx.save(model, str(model_path))
    print(f"Saved model to: {model_path}")

    # Generate and save input tensor (1, 4) float32 as 16 raw bytes
    test_input = generate_test_input()
    raw_bytes = test_input.tobytes()
    byte_count = len(raw_bytes)
    print(f"Input shape: {test_input.shape}, dtype: {test_input.dtype}, raw bytes: {byte_count}")
    assert byte_count == 16, f"Expected 16 raw bytes for (1,4) float32, got {byte_count} bytes"

    with open(input_bin_path, "wb") as f:
        f.write(raw_bytes)
    print(f"Saved raw input tensor ({byte_count} bytes) to: {input_bin_path}")
    print("Done! Fixture generation completed successfully.")

if __name__ == "__main__":
    main()
