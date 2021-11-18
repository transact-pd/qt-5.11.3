// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IDBValue_h
#define IDBValue_h

#include <memory>

#include "base/macros.h"
#include "base/memory/scoped_refptr.h"
#include "modules/ModulesExport.h"
#include "modules/indexeddb/IDBKey.h"
#include "modules/indexeddb/IDBKeyPath.h"
#include "platform/SharedBuffer.h"
#include "public/platform/WebVector.h"

namespace blink {

class BlobDataHandle;
class SerializedScriptValue;
class WebBlobInfo;
class WebIDBValue;

// Represents an IndexedDB Object Store value retrieved from the backing store.
//
// For most purposes, the backing store represents each IndexedDB value as wire
// data (a vector of bytes produced by SerializedScriptValue) and attached Blobs
// (a vector of Blobs).
//
// Object stores with auto-incrementing primary keys are a special case. To
// guarantee that we generate unique sequential numbers, the primary keys for
// these values are generated by the backing store. In this case, the primary
// key must be stored along the wire data. The backing store cannot invoke
// SerializedScriptValue, so it cannot inject the primary key into the wire
// bytes. Instead, when the values are read, Blink receives the primary keys
// along the IndexedDB values, and is responsible for injecting the keys into
// the values before returning them to the user.
class MODULES_EXPORT IDBValue final {
 public:
  // Creates an IDBValue from backing store information.
  static std::unique_ptr<IDBValue> Create(const WebData&,
                                          const WebVector<WebBlobInfo>&);

  // Used by IDBValueUnwrapper tests.
  static std::unique_ptr<IDBValue> Create(
      scoped_refptr<SharedBuffer> unwrapped_data,
      Vector<scoped_refptr<BlobDataHandle>>,
      Vector<WebBlobInfo>);

  ~IDBValue();

  size_t DataSize() const { return data_ ? data_->size() : 0; }

  bool IsNull() const;
  Vector<String> GetUUIDs() const;
  scoped_refptr<SerializedScriptValue> CreateSerializedValue() const;
  const Vector<WebBlobInfo>& BlobInfo() const { return blob_info_; }
  const IDBKey* PrimaryKey() const { return primary_key_.get(); }
  const IDBKeyPath& KeyPath() const { return key_path_; }

  // Injects a primary key into a value coming from the backend.
  void SetInjectedPrimaryKey(std::unique_ptr<IDBKey> primary_key,
                             IDBKeyPath primary_key_path) {
    primary_key_ = std::move(primary_key);
    key_path_ = std::move(primary_key_path);
  }

  // Sets the V8 isolate that this value's database lives in.
  //
  // Associating a V8 isolate informs V8's garbage collection about the memory
  // used by the IDBValue's wire data. This is crucial for V8 to be able to
  // schedule garbage collection in a timely manner when large IndexedDB values
  // are in use.
  void SetIsolate(v8::Isolate*);

  // Replaces this value's wire bytes.
  //
  // Used when unwrapping a value whose wire bytes are stored in a Blob.
  void SetData(scoped_refptr<SharedBuffer>);

  // Removes the last Blob from the IDBValue.
  //
  // When wire bytes are wrapped into a Blob, the Blob is appended at the end of
  // the IndexedDB value sent to the backing store. Conversely, removing the
  // last Blob from an IDBValue is used when unwrapping values.
  scoped_refptr<BlobDataHandle> TakeLastBlob();

#if DCHECK_IS_ON()
  // Called by WebIDBValue to inform IDBValue of owneship changes.
  void SetIsOwnedByWebIDBValue(bool);
#endif  // DCHECK_IS_ON()

 private:
  DISALLOW_COPY_AND_ASSIGN(IDBValue);

  friend class IDBValueUnwrapper;

  IDBValue(const WebData&, const WebVector<WebBlobInfo>&);
  IDBValue(scoped_refptr<SharedBuffer> unwrapped_data,
           Vector<scoped_refptr<BlobDataHandle>>,
           Vector<WebBlobInfo>);

  // Keep this private to prevent new refs because we manually bookkeep the
  // memory to V8.
  scoped_refptr<SharedBuffer> data_;

  Vector<scoped_refptr<BlobDataHandle>> blob_data_;
  Vector<WebBlobInfo> blob_info_;

  std::unique_ptr<IDBKey> primary_key_;
  IDBKeyPath key_path_;

  // Used to register memory externally allocated by the WebIDBValue, and to
  // unregister that memory in the destructor. Unused in other construction
  // paths.
  v8::Isolate* isolate_ = nullptr;
  int64_t external_allocated_size_ = 0;
#if DCHECK_IS_ON()
  // True if the IDBValue is owned by a WebIDBValue.
  //
  // IDBValue instances that are not owned by WebIDBValue are owned by Blink
  // objects, and must have a V8 isolate associated with them.
  bool is_owned_by_web_idb_value_ = false;
#endif  // DCHECK_IS_ON()
};

}  // namespace blink

#endif