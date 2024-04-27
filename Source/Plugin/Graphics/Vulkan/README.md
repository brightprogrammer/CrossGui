# CrossGui Vulkan Graphics Plugin

[[**Sat, 27th April 2024**]]

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
