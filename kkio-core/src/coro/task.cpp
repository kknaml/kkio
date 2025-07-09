module;

import std;
import kkio.traits;
import kkio.coro.cancellation;
import kkio.coro.base;

module kkio.coro.task;

namespace kkio::coro {

    auto IOContext::get_cancel_token() -> std::shared_ptr<CancellationToken> * {
        return &reinterpret_cast<detail::BaseTaskPromise<void> *>(this)->cancel_token;
    }

}