-- pp = new_path_planner()

vis = new_visualizer()

image = load_image("./images/something.png")
gp = train_gp(image)

sampler = new_pd_sampler(5000, 2000)
-- Can overwrite default hyperparameters with:
sampler.alpha = 12
sample_gp(sampler, gp)
-- can also:
sample_image(sampler, image)

diagram = generate_voronoi(sampler.out_points)
pp = new_path_planner(diagram, gp, { 0, 0 })
advance_planner(pp)

set_vis_diagram(vis, diagram)
set_vis_pp(vis, pp)
set_vis_gp(vis, gp)

render_frame(vis)
