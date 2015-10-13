#include <algorithm>
#include <cfloat>
#include <cmath>
#include <vector>

#include "caffe/layer.hpp"
#include "caffe/util/io.hpp"
#include "caffe/util/math_functions.hpp"
#include "caffe/vision_layers.hpp"

namespace caffe {

template <typename Dtype>
void PairwiseRankingLossLayer<Dtype>::Forward_cpu(const vector<Blob<Dtype>*>& bottom,
    const vector<Blob<Dtype>*>& top) {
  const Dtype* pos_sim = bottom[0]->cpu_data();
  const Dtype* neg_sim = bottom[1]->cpu_data();
  Dtype* per_triplet_loss = bottom[0]->mutable_cpu_diff();
  int count = bottom[0]->count();

  Dtype* loss = top[0]->mutable_cpu_data();
  loss[0] = 0;
  for (int i=0; i<count; ++i) {
    per_triplet_loss[i] = std::max(Dtype(0),
        this->layer_param_.pairwise_ranking_loss_param().margin()
        - pos_sim[i] + neg_sim[i]);
    loss[0] += per_triplet_loss[i];
  }
  loss[0] /= count;
}

template <typename Dtype>
void PairwiseRankingLossLayer<Dtype>::Backward_cpu(const vector<Blob<Dtype>*>& top,
    const vector<bool>& propagate_down, const vector<Blob<Dtype>*>& bottom) {
  if (propagate_down[0]) {
    Dtype* pos_diff = bottom[0]->mutable_cpu_diff();
    Dtype* neg_diff = bottom[1]->mutable_cpu_diff();
    int count = bottom[0]->count();
    for (int i=0; i<count; ++i) {
      pos_diff[i] = pos_diff[i] ? -1 : 0;
      neg_diff[i] = pos_diff[i] ?  1 : 0;
    }
  }
}

INSTANTIATE_CLASS(PairwiseRankingLossLayer);
REGISTER_LAYER_CLASS(PairwiseRankingLoss);

}  // namespace caffe
