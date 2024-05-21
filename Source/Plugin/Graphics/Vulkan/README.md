# CrossGui Vulkan Graphics Plugin

## [[**Tue, 21st May 2024**]]

Wow, one month of vacation almost gone! The commits near this date achieve batch
rendering support. While writing this some refactorings still remain, like moving
the batch data to a new class named `BatchRenderer`, and move the drawing functions
there as well.

Here are some very important lessons learned :

- I initially thought that I'd send vertex data and instance as UBO and access those
  using `gl_VertexID` and `gl_InstanceID` invariants in GLSL. Turns out the actual
  (and probably the best solution for the moment) is way better than this. I just
  needed to send two vertex buffers. One will change per instance and one per vertex.
  This also removes the alignment requirements required by uniform buffers and saves a lot
  of memory because alignment almost doubled the size almost always. (44->80,128)
- Maybe I was overcomplicating things. KISS (Keep It Simple Stupid)
- For creating shapes, I initially was taking one vertex at a time, now I just take the
  complete mesh from the user and force them to use index buffers. They save memory a lot.
- Synchronization is very vert important in Vulkan. The bug mentioned in [this](https://github.com/brightprogrammer/CrossGui/commit/65c85af99b8302c1945a4f8c12d7359e95977afd)
  commit is due to this sync problem. To be more precise, the actual bug is [here](https://github.com/brightprogrammer/CrossGui/blob/65c85af99b8302c1945a4f8c12d7359e95977afd/Source/Plugin/Graphics/Vulkan/Renderer.c#L183)
  because the memory access is not synchronized. I update the same vertex buffer many
  times in the same draw call. Due to this, I was updating the buffer even when it was
  being in use currently. To fix this, I just added separate instance buffers for each batch,
  and now we have smooth rendering.

After some more refactoring this WIP will be closed and we'll move on to dynamic device memory
management or some other interesting thing.

One thing from our refactoring goals cannot be completed (atleast with my current knowledge).
We cannot render everything in a single draw call now. The draw calls are broken into multiple
batches where the number of batch depends on the number of shapes added to the mesh manager.

The shapes will be global and will be accessible by all instances of `BatchRenderer`, but instance
data is unique for each `GraphicsContext` and must be separate for each one. Therefore the concept of
a separate `BatchRenderer` is required.

As for staging and device local buffers, we can do that only for shapes because they don't change
a lot. For now, the design does not allow the user-code to modify or delete any shape that's uploaded
to the GPU. This means the user-code must be really careful with what they upload. Or they can even be
careless, if they want to upload 1000s of shapes, which will eventually happen when we enter
Text rendering (probably).

## [[**Wed, 15th May 2024**]]

Been thinking of refactoring the plugin to add one draw call rendering, but had a
fever for past three-four days. I took medicine yesterday and now feeling a bit better.

The refactoring goals goes like this :

- We need dynamic device memory for dynamically resizing the vertex and uniform buffers and
  copy-pasting the shape, vertex, etc.. data to the GPU.
- Allow creation of staging buffers and sending of shape data as uniform data that is local
  to the GPU. This will make sure that shape data is fast accessible.
  NOTE: Shape data is not susceptible to frequent changes.
- Send an array of `XuiRenderShape` data as uniform buffer to GPU. This shape data is not
  supposed to change a lot.
  We want the shape data to be changeable however. So we'll provide features like

   - xui_render_shape2d_clear(shape)
   - xui_render_shape2d_add_vertex(shape, x, y, z, color_id)
   - xui_render_shape2d_upload_to_gpu(shape)

  The final upload to gpu call will instruct the plugin to send the shape data to the GPU. The plugin
  can either send it right at that moment, or we can setup a job queue that is flushed once the queue
  is full.

- Render EVERYTHING in a SINGLE DRAW CALL by sending variable data as vertex buffers,
  and shape data as an enumerated value inside vertex buffer that indexes into the array
  of shapes in uniform.

## [[**Thu, 9th May 2024**]]

- Added helper method for naming of vulkan handles. This helps the validation layer
  provide some extra passed information in validation messages.
- Drawing command works

I now have to figure out how to draw the whole UI in a single frame. If not then
the `draw_ui` command needs to be issued as many times as there are swapchain images,
after `XuiGraphicsPlugin::clear` command. This is to make sure all framebuffers have
same content. This idea is not very efficient.

TODO: draw the whole UI using a single draw call. Prolly I can use instanced rendering.

## [[**Wed, 8nd May 2024**]]

Added `gfx_clear` in graphics api to allow clearing of swapchain images on demand.
If GUI is drawn in retained mode, we'd prefer not clearing images ever loop.
User-code can use `gfx_clear` op to clear the screen.

## [[**Thu, 2nd May 2024**]] - 2

Added `XuiRenderStatus` enum to be returned by `draw_xyz` method to give more
verbose error handling options to user-code.

- A value of `XUI_RENDER_STATUS_ERR` means irrecoverable error.
- A value of `XUI_RENDER_STATUS_CONTINUE` is generally issued on swapchain resize or
    suboptimal
- A value of `XUI_RENDER_STATUS_OK` means success.

## [[**Thu, 2nd May 2024**]] - 1

Finally refactoring finished! Phew!! A 9 commit long refactor work!
Here are the changes in this final commit that solves all problems:

- Removed `RenderTarget` concept, and moved on with approach followed by tutorials.
  I have now a `FrameData` that stores sync objects and command objects. I realize
  after some struggle that it's not possible to separate the command and sync objects,
  because sync objects _is_ for the command recording itself.
- `SwapchainSyncObjects` and `RenderTarget` is now merged into `FrameData` with
  framebuffers being stored separately in the `RenderPass`
- `FRAME_LIMIT` is defined as 2 for now on top of `RenderPass.c` which can be changed,
  but I don't think I'll require changing that anytime soon.

Some things might still change, but overall the plugin is stable ;-)

## [[**Wed, 1st May 2024**]]

- Moved `VkSurfaceKHR` from `XuiGraphicsContext` to `Swapchain`. This resulted in
  on less parameter in `swapchain_xxxx` calls.
- Renamed `RenderTargetSyncObjects` to `SwapchainSyncObjects`, and moved from per
  `RenderTarget` association to direct association with `Swapchain`. This is under
  the assumption that only one sync object will be used at a time.
- Added two methods :
      - `swapchain_begin_frame` : To get next image index
      - `swapchain_end_frame` : To submit recorded commands for rendering and presentation
         to graphics queue.
- Each `RenderTarget` now has it's own `VkCommandPool` object for allocation of it's
  `VkCommandBuffer` objects. This decision is because of [this](https://docs.vulkan.org/samples/latest/samples/performance/command_buffer_usage/README.html#_resetting_the_command_pool)
   suggestion. I also read a similar suggestion in vkguide.

## [[**Mon, 29th April 2024**]]

Few more things added while refactoring in commits

- [650f9f6d1ee8765d449e93d8bb2b65071bd21aaa](https://github.com/brightprogrammer/CrossGui/commit/650f9f6d1ee8765d449e93d8bb2b65071bd21aaa)
- [51e7bdd992c72db4ef72947b7c6446d890d8c3fe](https://github.com/brightprogrammer/CrossGui/commit/51e7bdd992c72db4ef72947b7c6446d890d8c3fe)
- [06de3238ae9cec483971ce0372c24e7c4b3ca5ae](https://github.com/brightprogrammer/CrossGui/commit/06de3238ae9cec483971ce0372c24e7c4b3ca5ae)
- [de05dd3726b47c250a7dc68c7eeb7c991ae23a2a](https://github.com/brightprogrammer/CrossGui/commit/de05dd3726b47c250a7dc68c7eeb7c991ae23a2a)

Explanation :

- `RenderTarget` : A `RenderTarget` is where your GPU outputs it's rendering results in different
   subpasses and at the end of the `RenderPass`.
- `RenderPass` : A `RenderPass` handles how different `GraphicsPipeline` objects are executed.
   In other graphics libraries, the `GraphicsPipeline` might be known as `GraphicsPSO` or the
   Graphics Pipeline State Object.
- `GraphicsPipeline` : It is what you think it is :-)

Diagram below explains relationship between `Swapchain`, `RenderPass` and `RenderTargets`, as well
as `GraphicsPipeline`.

![](Assets/Images/swapchain-renderpass-rendertarget-relationship-diagram.svg)

The `Swapchain` object stores callback methods provided by the `RenderPass` object that was initialized
using it. The callback method basically is called whenever the swapchain is `reinit`-ed
(like swapchain recreated), to handle the recreation of `RenderTarget` objects as well onwed by
the corresponding `RenderPass`.

This allows the `RenderPass` to own it's `RenderTarget` objects, and it to handle `Swapchain`
reinit events as well while not being very tightly coupled with a `Swapchain`.

## [[**Sat, 27th April 2024**]]

The Vulkan Graphics Plugin works by maintaining a global `Vulkan` state. This allows all parts of
the plugin to have access to commonly shared instance and commonly shared `Device` object. The
shared object is then used to create everything that plugin requires, including surface, swapchain,
buffers, images, framebuffers, renderpasses, descriptor sets/pools, command buffer sets/pools,
in draw calls, etc...

Therefore for now the renderering is performed only on one single device. If we allow each surface
to have it's own logical device handle, then we'd need to create all other resources using that same
device.

So, right now, I don't know the best course of action :

- Should I create all resources like descriptor sets, images, buffers, pipelines, etc... when a
  new window is created?
- Should I just keep using one single device (because that makes things easy and fast).

As an alternative solution is that I should leave out scope for future modification, where I can
allow a `Surface` to have a custom `VkDevice` handle if default `Device` does not support presenting
to it. This seems like a rare case though. Even though I create a `Device` for each `Surface`, that
would mean I'll have to create all objects again and then store it somewhere? Where? That does not
make any sense at all! Because, this would then mean I either store these objects in the `Surface`
itself or somewhere else like `Renderer2D`.

Due to the above decision, when reading the code, keep an eye on whether I'm getting `VkDevice`,
and `VkInstance` handle from function parameter or from `Vulkan` global state :-)
