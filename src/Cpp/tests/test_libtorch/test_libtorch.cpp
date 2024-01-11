#include <torch/script.h> // One-stop header.

#include <iostream>
#include <memory>
#include <tuple>
#include <vector>

int main(int argc, const char* argv[]) {
  if (argc != 2) {
    std::cerr << "usage: example-app <path-to-exported-script-module>\n";
    return -1;
  }


  torch::jit::script::Module module;
  try {
    // Deserialize the ScriptModule from a file using torch::jit::load().
    module = torch::jit::load(argv[1]);
  }
  catch (const c10::Error& e) {
    std::cerr << "error loading the model\n";
    return -1;
  }

  std::cout << "ok\n";

  // Create a vector of inputs.
  std::vector<torch::jit::IValue> inputs;
  // inputs.push_back(torch::ones({ 1, 3, 224, 224 }));  // Example model
  inputs.push_back(torch::ones({ 1, 60 }));  // Poker model

  // Execute the model and turn its output into a tensor.
  auto output = module.forward(inputs).toTuple();
  at::Tensor actions = output->elements()[0].toTensor();
  at::Tensor bet = output->elements()[1].toTensor();
  std::cout << actions << '\n';
  std::cout << bet << '\n';
  std::cout << at::argmax(actions) << '\n';
  std::cout << bet.item<float>() << '\n';
  actions[0][3] = -99;
  std::cout << actions << '\n';
  bet[0] = -98;
  std::cout << bet << '\n';
  std::vector<double> input = { 0,1,2,3 };
  at::Tensor tensor_input = torch::tensor(input).unsqueeze(0);
  std::cout << tensor_input << '\n';
  std::cout << torch::softmax(tensor_input, 1) << '\n';
  tensor_input[0][3] = -1.0;
  std::cout << torch::softmax(tensor_input, 1) << '\n';
  std::cout << at::min(actions) << '\n';
  double x = 20;
  x += at::min(actions).item<double>();
  std::cout << x << '\n';
}