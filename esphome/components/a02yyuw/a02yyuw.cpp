// Datasheet https://wiki.dfrobot.com/_A02YYUW_Waterproof_Ultrasonic_Sensor_SKU_SEN0311

#include "a02yyuw.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace a02yyuw {

static const char *const TAG = "a02yyuw.sensor";
static const int MAX_DISTANCE = 4500;
static int last_good = MAX_DISTANCE;

void A02yyuwComponent::loop() {
  uint8_t data;
  while (this->available() > 0) {
    this->read_byte(&data);
    if (this->buffer_.empty() && (data != 0xff))
      continue;
    buffer_.push_back(data);
    if (this->buffer_.size() == 4)
      this->check_buffer_();
  }
}

void A02yyuwComponent::check_buffer_() {
  uint8_t checksum = this->buffer_[0] + this->buffer_[1] + this->buffer_[2];
  if (this->buffer_[3] == checksum) {
    float distance = (this->buffer_[1] << 8) + this->buffer_[2];
    if (distance > 0) {
      ESP_LOGV(TAG, "Distance from sensor: %f mm", distance);
      this->publish_state(distance);
      last_good = distance;
    } else {
      float zero_distance = MAX_DISTANCE;
      this->publish_state(zero_distance);
    }
  } else {
    ESP_LOGV(TAG, "checksum failed: %02x != %02x", checksum, this->buffer_[3]);
  }
  this->buffer_.clear();
}

void A02yyuwComponent::dump_config() { 
  LOG_SENSOR("", "A02yyuw Sensor", this); 
}

}  // namespace a02yyuw
}  // namespace esphome
