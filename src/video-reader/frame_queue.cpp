#include "../headers.h"

// Add new frame to end of frame queue
void frame_queue_push(VideoReader *video_reader, uint8_t *new_frame)
{
  // Don't add node if queue is already at max length
  if (video_reader->frame_queue_length < MAX_QUEUE_LENGTH)
  {
    // Base case where frame_queue's initial frame_data will be NULL
    if (video_reader->frame_queue->frame_data == NULL)
    {
      video_reader->frame_queue->frame_data = new_frame;
    }
    else
    {
      FrameNode *node = video_reader->frame_queue;

      // Find the end of the queue i.e. where a node's next_frame is NULL
      while (node->next_frame != NULL)
      {
        node = node->next_frame;
      }

      // Set last node's frame_data to new frame data
      node->frame_data = new_frame;

      // Set last node's next_frame to an empty FrameNode
      node->next_frame = new FrameNode{NULL, NULL};
    }

    // Increase queue length so we know how many frames there are
    video_reader->frame_queue_length++;
  }
}

// Assign a pointer to the first frame in the queue and remove that frame from the queue
void frame_queue_consume(VideoReader *video_reader, FrameNode **node_out)
{
  // Free the node's current frame data before assigning it new data
  if (*node_out != NULL)
  {
    frame_queue_cleanup_node(*node_out);
  }

  if (video_reader->frame_queue_length > 0 && video_reader->frame_queue != NULL)
  {
    // Assign first frame in the queue to external pointer
    *node_out = video_reader->frame_queue;

    // Set first frame node pointer in the queue to the next frame node,
    // effectively removing the head from the queue
    video_reader->frame_queue = video_reader->frame_queue->next_frame;
    video_reader->frame_queue_length--;
  }
}

void frame_queue_cleanup_node(FrameNode *node)
{
  free(node->frame_data);
  node = NULL;
}

void frame_queue_cleanup(FrameNode *node)
{
  // Recursively free all nodes in a frame queue
  if (node != NULL && node->frame_data != NULL)
  {
    frame_queue_cleanup_node(node);
    frame_queue_cleanup(node->next_frame);
  }
}
