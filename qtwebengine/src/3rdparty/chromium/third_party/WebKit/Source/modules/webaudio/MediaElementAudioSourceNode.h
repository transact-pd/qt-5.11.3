/*
 * Copyright (C) 2011, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef MediaElementAudioSourceNode_h
#define MediaElementAudioSourceNode_h

#include <memory>
#include "base/memory/scoped_refptr.h"
#include "modules/webaudio/AudioNode.h"
#include "platform/audio/AudioSourceProviderClient.h"
#include "platform/audio/MultiChannelResampler.h"
#include "platform/wtf/ThreadingPrimitives.h"

namespace blink {

class BaseAudioContext;
class HTMLMediaElement;
class MediaElementAudioSourceOptions;

class MediaElementAudioSourceHandler final : public AudioHandler {
 public:
  static scoped_refptr<MediaElementAudioSourceHandler> Create(
      AudioNode&,
      HTMLMediaElement&);
  ~MediaElementAudioSourceHandler() override;

  HTMLMediaElement* MediaElement() const;

  // AudioHandler
  void Dispose() override;
  void Process(size_t frames_to_process) override;

  // AudioNode
  double TailTime() const override { return 0; }
  double LatencyTime() const override { return 0; }

  // Helpers for AudioSourceProviderClient implementation of
  // MediaElementAudioSourceNode.
  void SetFormat(size_t number_of_channels, float sample_rate);
  void lock();
  void unlock();

 private:
  MediaElementAudioSourceHandler(AudioNode&, HTMLMediaElement&);
  // As an audio source, we will never propagate silence.
  bool PropagatesSilence() const override { return false; }

  // Returns true if the origin of the media element is tainted so that the
  // audio should be muted when playing through WebAudio.
  bool WouldTaintOrigin();

  // Print warning if CORS restrictions cause MediaElementAudioSource to output
  // zeroes.
  void PrintCORSMessage(const String& message);

  // This Persistent doesn't make a reference cycle. The reference from
  // HTMLMediaElement to AudioSourceProvideClient, which
  // MediaElementAudioSourceNode implements, is weak.
  //
  // It is accessed by both audio and main thread. TODO: we really should
  // try to minimize or avoid the audio thread touching this element.
  CrossThreadPersistent<HTMLMediaElement> media_element_;
  Mutex process_lock_;

  unsigned source_number_of_channels_;
  double source_sample_rate_;

  std::unique_ptr<MultiChannelResampler> multi_channel_resampler_;

  scoped_refptr<WebTaskRunner> task_runner_;

  // True if the orgin would be tainted by the media element.  In this case,
  // this node outputs silence.  This can happen if the media element source is
  // a cross-origin source which we're not allowed to access due to CORS
  // restrictions.
  bool is_origin_tainted_;
};

class MediaElementAudioSourceNode final : public AudioNode,
                                          public AudioSourceProviderClient {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(MediaElementAudioSourceNode);

 public:
  static MediaElementAudioSourceNode* Create(BaseAudioContext&,
                                             HTMLMediaElement&,
                                             ExceptionState&);
  static MediaElementAudioSourceNode* Create(
      BaseAudioContext*,
      const MediaElementAudioSourceOptions&,
      ExceptionState&);

  virtual void Trace(blink::Visitor*);
  MediaElementAudioSourceHandler& GetMediaElementAudioSourceHandler() const;

  HTMLMediaElement* mediaElement() const;

  // AudioSourceProviderClient functions:
  void SetFormat(size_t number_of_channels, float sample_rate) override;
  void lock() override;
  void unlock() override;

 private:
  MediaElementAudioSourceNode(BaseAudioContext&, HTMLMediaElement&);
};

}  // namespace blink

#endif  // MediaElementAudioSourceNode_h