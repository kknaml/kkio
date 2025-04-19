module kkio.runtime.worker;

import std;

namespace kkio::runtime {

    std::vector<StealingWorker*> StealingWorker::workers_ = {};

}