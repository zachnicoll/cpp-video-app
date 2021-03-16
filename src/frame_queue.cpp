#include "headers.h"

// Add new frame to end of frame queue
void frame_queue_push(VideoReader *video_reader, uint8_t *new_frame)
{
  if (video_reader->frame_queue_length < MAX_QUEUE_LENGTH)
  {
    if (video_reader->frame_queue->frame_data == NULL)
    {
      video_reader->frame_queue->frame_data = new_frame;
    }
    else
    {
      FrameNode *node = video_reader->frame_queue;

      while (node->next_frame != NULL)
      {
        node = node->next_frame;
      }

      node->frame_data = new_frame;
      node->next_frame = new FrameNode;
      node->next_frame->frame_data = NULL;
      node->next_frame->next_frame = NULL;
    }
    video_reader->frame_queue_length++;
  }
}

// Assign a pointer to the first frame in the queue and remove that frame from the queue
void frame_queue_consume(VideoReader *video_reader, FrameNode **node_out)
{
  if (video_reader->frame_queue_length > 0)
  {
    *node_out = video_reader->frame_queue;
    video_reader->frame_queue = video_reader->frame_queue->next_frame;
    video_reader->frame_queue_length--;
  }
}

void frame_queue_cleanup_node(FrameNode *node)
{
  free(node->frame_data);
}

void frame_queue_cleanup(FrameNode *node)
{
  if (node->frame_data != NULL) {
    frame_queue_cleanup_node(node);
    frame_queue_cleanup(node->next_frame);
  }
}
