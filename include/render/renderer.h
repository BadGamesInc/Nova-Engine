//
// Created by gregorym on 5/10/26.
//

#ifndef NOVA_ENGINE_RENDERER_H
#define NOVA_ENGINE_RENDERER_H

#define FOV 70
#define NEAR_PLANE 0.1
#define FAR_PLANE 1000

void renderer_init (void);
void renderer_render_main (void);
void renderer_cleanup (void);

#endif //NOVA_ENGINE_RENDERER_H
