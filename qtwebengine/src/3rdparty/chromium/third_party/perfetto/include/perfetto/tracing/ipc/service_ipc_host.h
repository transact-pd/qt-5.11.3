/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INCLUDE_PERFETTO_TRACING_IPC_SERVICE_IPC_HOST_H_
#define INCLUDE_PERFETTO_TRACING_IPC_SERVICE_IPC_HOST_H_

#include <memory>

#include "perfetto/tracing/core/basic_types.h"

namespace perfetto {
namespace base {
class TaskRunner;
}  // namespace base.

class Service;

// Creates an instance of the service (business logic + UNIX socket transport).
// Exposed to:
//   The code in the tracing client that will host the service e.g., traced.
// Implemented in:
//   src/ipc/service/service_ipc_host_impl.cc
class ServiceIPCHost {
 public:
  static std::unique_ptr<ServiceIPCHost> CreateInstance(base::TaskRunner*);
  virtual ~ServiceIPCHost();

  // Start listening on the Producer & Consumer ports. Returns false in case of
  // failure (e.g., something else is listening on |socket_name|).
  virtual bool Start(const char* producer_socket_name) = 0;

 protected:
  ServiceIPCHost();

 private:
  ServiceIPCHost(const ServiceIPCHost&) = delete;
  ServiceIPCHost& operator=(const ServiceIPCHost&) = delete;
};

}  // namespace perfetto

#endif  // INCLUDE_PERFETTO_TRACING_IPC_SERVICE_IPC_HOST_H_
