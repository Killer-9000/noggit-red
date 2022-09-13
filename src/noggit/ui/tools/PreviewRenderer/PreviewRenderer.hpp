#ifndef NOGGIT_PREVIEWRENDERER_HPP
#define NOGGIT_PREVIEWRENDERER_HPP

#include <noggit/BoolToggleProperty.hpp>
#include <noggit/Camera.hpp>
#include <noggit/ContextObject.hpp>
#include <noggit/Model.h>
#include <noggit/ModelInstance.h>
#include <noggit/rendering/LiquidTextureManager.hpp>
#include <noggit/rendering/Primitives.hpp>
#include <noggit/ui/tools/ViewportManager/ViewportManager.hpp>
#include <noggit/WMO.h>
#include <noggit/WMOInstance.h>

#include <QOpenGLWidget>
#include <QSettings>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObjectFormat>
#include <QOffscreenSurface>
#include <QPixmap>

#include <vector>


namespace Noggit::Ui::Tools
{
	class PreviewRenderer : public Noggit::Ui::Tools::ViewportManager::Viewport
  {
    Q_OBJECT

  public:
    explicit PreviewRenderer(int width, int height, Noggit::NoggitRenderContext context, QWidget* parent = nullptr);

    void resetCamera(float x = 0.f, float y = 0.f, float z = 0.f, float roll = 0.f, float yaw = 120.f, float pitch = 20.f);
    QPixmap* renderToPixmap();

    // Will clear all other models, and set just the one.
    virtual void setModel(const std::string& filename);
    void setModelOffscreen(const std::string& filename);

    // Will add onto the list of models to render.
    virtual void addModel(const std::string& filename, glm::vec3 offset = glm::vec3(0, 0, 0), glm::vec3 rotation = glm::vec3(0, 0, 0), float scale = 1.0f);
    void addModelOffscreen(const std::string& filename, glm::vec3 offset = glm::vec3(0, 0, 0), glm::vec3 rotation = glm::vec3(0, 0, 0), float scale = 1.0f);

    virtual void clearModels()
    {
      OpenGL::context::save_current_context const context_save(::gl);
      _offscreen_context.makeCurrent(&_offscreen_surface);
      OpenGL::context::scoped_setter const context_set(::gl, &_offscreen_context);

      _filenames.clear();
      _model_instances.clear();
      _wmo_instances.clear();
    }

    virtual void setPrefab(const std::string& filename) {};

    void setLightDirection(float y, float z);

    BoolToggleProperty _draw_models = {true};
    BoolToggleProperty _draw_wmo = {true};
    BoolToggleProperty _draw_particles = {true};
    BoolToggleProperty _draw_animated = {true};
    BoolToggleProperty _draw_boxes = {false};
    BoolToggleProperty _draw_grid = {false};

    void unloadOpenglData() override;

  protected:

    bool _offscreen_mode = true;
    Noggit::Camera _camera;
    QSettings* _settings;
    std::vector<std::string> _filenames;

    std::unique_ptr<OpenGL::program> _m2_program;
    std::unique_ptr<OpenGL::program> _m2_instanced_program;
    std::unique_ptr<OpenGL::program> _m2_particles_program;
    std::unique_ptr<OpenGL::program> _m2_ribbons_program;
    std::unique_ptr<OpenGL::program> _m2_box_program;
    std::unique_ptr<OpenGL::program> _wmo_program;
    std::unique_ptr<OpenGL::program> _liquid_program;

    std::vector<ModelInstance> _model_instances;
    std::vector<WMOInstance> _wmo_instances;

    Noggit::Rendering::Primitives::Grid _grid;

    float _animtime = 0.f;

    std::vector<glm::vec3> calcSceneExtents();
    virtual void draw();
    virtual void tick(float dt);
    virtual glm::mat4x4 model_view() const;
    virtual glm::mat4x4 projection() const;
    virtual float aspect_ratio() const;

    void update_emitters(float dt);

    void upload();

    void unload();

    void updateLightingUniformBlock();

    void updateMVPUniformBlock(const glm::mat4x4& model_view, const glm::mat4x4& projection);

  private:
    int _width;
    int _height;

    std::map<std::tuple<std::string, int, int>, QPixmap> _cache;

    QOpenGLContext _offscreen_context;
    QOpenGLFramebufferObjectFormat _fmt;
    QOffscreenSurface _offscreen_surface;

    glm::vec3 _background_color;
    glm::vec3 _diffuse_light;
    glm::vec3 _ambient_light;
    glm::vec3 _light_dir;

    OpenGL::Scoped::deferred_upload_buffers<2> _buffers;
    GLuint const& _mvp_ubo = _buffers[0];
    GLuint const& _lighting_ubo = _buffers[1];

    OpenGL::MVPUniformBlock _mvp_ubo_data;
    OpenGL::LightingUniformBlock _lighting_ubo_data;

    Noggit::Rendering::LiquidTextureManager _liquid_texture_manager;

    bool _uploaded = false;
    bool _lighting_needs_update = true;

  };

}


#endif //NOGGIT_PREVIEWRENDERER_HPP
