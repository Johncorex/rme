#include "main.h"
#include "welcome_dialog.h"
#include "settings.h"

WelcomeDialog::WelcomeDialog(const wxString &titleText,
                             const wxString &versionText,
                             const wxBitmap &rmeLogo,
                             const std::vector<wxString> &recentFiles)
        : wxDialog(nullptr, wxID_ANY, "", wxDefaultPosition, wxSize(800, 500)), m_recentMapPath("") {
    Centre();
    wxColour baseColour = wxColor(250, 250, 250);
    m_welcomeDialogPanel = newd WelcomeDialogPanel(this,
                                                   GetClientSize(),
                                                   titleText,
                                                   versionText,
                                                   baseColour,
                                                   rmeLogo,
                                                   recentFiles);
}

void WelcomeDialog::OnButtonClicked(wxMouseEvent &event) {
    auto *button = dynamic_cast<WelcomeDialogButtonButton *>(event.GetEventObject());
    wxSize buttonSize = button->GetSize();
    wxPoint clickPoint = event.GetPosition();
    if (clickPoint.x > 0 && clickPoint.x < buttonSize.x && clickPoint.y > 0 && clickPoint.y < buttonSize.x) {
        if (button->GetAction() == wxID_OPEN) {
            wxString wildcard = g_settings.getInteger(Config::USE_OTGZ) != 0 ?
                                "(*.otbm;*.otgz)|*.otbm;*.otgz" :
                                "(*.otbm)|*.otbm|Compressed OpenTibia Binary Map (*.otgz)|*.otgz";
            wxFileDialog fileDialog(this, "Open map file", "", "", wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
            if (fileDialog.ShowModal() == wxID_OK) {
                m_recentMapPath = fileDialog.GetPath();
                EndModal(wxID_FILE);
            }
        } else {
            EndModal(button->GetAction());
        }
    }
}

void WelcomeDialog::OnRecentItemClicked(wxMouseEvent &event) {
    auto *recentItem = dynamic_cast<RecentItem *>(event.GetEventObject());
    wxSize buttonSize = recentItem->GetSize();
    wxPoint clickPoint = event.GetPosition();
    if (clickPoint.x > 0 && clickPoint.x < buttonSize.x && clickPoint.y > 0 && clickPoint.y < buttonSize.x) {
        m_recentMapPath = recentItem->GetText();
        EndModal(wxID_FILE);
    }
}

WelcomeDialogPanel::WelcomeDialogPanel(WelcomeDialog *dialog,
                                       const wxSize &size,
                                       const wxString &titleText,
                                       const wxString &versionText,
                                       const wxColour &baseColour,
                                       const wxBitmap &rmeLogo,
                                       const std::vector<wxString> &recentFiles)
        : wxPanel(dialog),
          m_rmeLogo(rmeLogo),
          m_titleText(titleText),
          m_versionText(versionText),
          m_textColour(baseColour.ChangeLightness(40)) {

    SetBackgroundColour(baseColour);

    newd RecentMapsPanel(this,
                         dialog,
                         wxPoint(size.x / 2, 0),
                         wxSize(size.x / 2, size.y),
                         baseColour,
                         recentFiles);

    wxSize buttonSize(150, 40);
    wxColour buttonBaseColour = baseColour.ChangeLightness(90);

    wxPoint newMapButtonPoint(size.x / 4 - buttonSize.x / 2, size.y / 2);
    auto *newMapButton = newd WelcomeDialogButtonButton(this, newMapButtonPoint, buttonSize, buttonBaseColour, "New");
    newMapButton->SetAction(wxID_NEW);
    newMapButton->Bind(wxEVT_LEFT_UP, &WelcomeDialog::OnButtonClicked, dialog);

    wxPoint openMapButtonPoint(size.x / 4 - buttonSize.x / 2, size.y / 2 + buttonSize.y + 10);
    auto *openMapButton = newd WelcomeDialogButtonButton(this, openMapButtonPoint, buttonSize, buttonBaseColour,
                                                         "Open");
    openMapButton->SetAction(wxID_OPEN);
    openMapButton->Bind(wxEVT_LEFT_UP, &WelcomeDialog::OnButtonClicked, dialog);

    Bind(wxEVT_PAINT, &WelcomeDialogPanel::OnPaint, this);
}

void WelcomeDialogPanel::OnPaint(wxPaintEvent &event) {
    wxPaintDC dc(this);

    dc.DrawBitmap(m_rmeLogo, wxPoint(GetSize().x / 4 - m_rmeLogo.GetWidth() / 2, 40), true);

    m_font.SetPointSize(24);
    dc.SetFont(m_font);
    wxSize headerSize = dc.GetTextExtent(m_titleText);
    wxSize headerPoint(GetSize().x / 4, GetSize().y / 4);
    dc.SetTextForeground(m_textColour);
    dc.DrawText(m_titleText, wxPoint(headerPoint.x - headerSize.x / 2, headerPoint.y));

    m_font.SetPointSize(14);
    dc.SetFont(m_font);
    wxSize versionSize = dc.GetTextExtent(m_versionText);
    dc.SetTextForeground(m_textColour.ChangeLightness(110));
    dc.DrawText(m_versionText, wxPoint(headerPoint.x - versionSize.x / 2, headerPoint.y + headerSize.y + 10));
}

WelcomeDialogButtonButton::WelcomeDialogButtonButton(wxWindow *parent,
                                                     const wxPoint &pos,
                                                     const wxSize &size,
                                                     const wxColour &baseColour,
                                                     const wxString &text)
        : wxPanel(parent, wxID_ANY, pos, size),
          m_action(wxID_CLOSE),
          m_font(),
          m_text(text),
          m_textColour(baseColour.ChangeLightness(40)),
          m_background(baseColour.ChangeLightness(96)),
          m_backgroundHover(baseColour.ChangeLightness(93)) {
    SetBackgroundColour(m_background);

    m_font.SetPointSize(12);

    Bind(wxEVT_PAINT, &WelcomeDialogButtonButton::OnPaint, this);
    Bind(wxEVT_ENTER_WINDOW, &WelcomeDialogButtonButton::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &WelcomeDialogButtonButton::OnMouseLeave, this);
}

void WelcomeDialogButtonButton::OnPaint(wxPaintEvent &event) {
    wxPaintDC dc(this);
    dc.SetFont(m_font);
    dc.SetTextForeground(m_textColour);
    wxSize textSize = dc.GetTextExtent(m_text);

    dc.DrawText(m_text, wxPoint(GetSize().x / 2 - textSize.x / 2, GetSize().y / 2 - textSize.y / 2));
}

void WelcomeDialogButtonButton::OnMouseEnter(wxMouseEvent &event) {
    SetBackgroundColour(m_backgroundHover);
    Refresh();
}

void WelcomeDialogButtonButton::OnMouseLeave(wxMouseEvent &event) {
    SetBackgroundColour(m_background);
    Refresh();
}

RecentMapsPanel::RecentMapsPanel(wxWindow *parent,
                                 WelcomeDialog *dialog,
                                 const wxPoint &pos,
                                 const wxSize &size,
                                 const wxColour &baseColour,
                                 const std::vector<wxString> &recentFiles)
        : wxPanel(parent, wxID_ANY, pos, size) {
    SetBackgroundColour(baseColour.ChangeLightness(99));

    int height = 40;
    int position = 0;
    for (const wxString &file : recentFiles) {
        auto *recentItem = newd RecentItem(this, wxPoint(0, position), wxSize(size.x, height), baseColour, file);
        recentItem->Bind(wxEVT_LEFT_UP, &WelcomeDialog::OnRecentItemClicked, dialog);
        position += height;
    }
}

RecentItem::RecentItem(wxWindow *parent,
                       const wxPoint &pos,
                       const wxSize &size,
                       const wxColour &baseColour,
                       const wxString &itemName)
        : wxPanel(parent, wxID_ANY, pos, size),
          m_baseColour(baseColour),
          m_textColour(baseColour.ChangeLightness(40)),
          m_background(baseColour.ChangeLightness(96)),
          m_backgroundHover(baseColour.ChangeLightness(93)),
          m_itemText(itemName) {
    SetBackgroundColour(m_background);

    Bind(wxEVT_PAINT, &RecentItem::OnPaint, this);
    Bind(wxEVT_ENTER_WINDOW, &RecentItem::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &RecentItem::OnMouseLeave, this);
}

void RecentItem::OnPaint(wxPaintEvent &event) {
    wxFont font;
    font.SetPointSize(12);

    wxPaintDC dc(this);

    dc.SetPen(wxPen(m_baseColour.ChangeLightness(99), 1));
    dc.DrawLine(0, 0, GetSize().x, 0);

    dc.SetFont(font);
    dc.SetTextForeground(m_textColour);
    wxSize textSize = dc.GetTextExtent(m_itemText);

    int x, y = GetSize().y / 2 - textSize.y / 2, padding = GetSize().y / 4;
    if (textSize.x + padding * 2 > GetSize().x) {
        x = GetSize().x - textSize.x - padding;
    } else {
        x = padding;
    }

    dc.DrawText(m_itemText, wxPoint(x, y));
}

void RecentItem::OnMouseEnter(wxMouseEvent &event) {
    SetBackgroundColour(m_backgroundHover);
    Refresh();
}

void RecentItem::OnMouseLeave(wxMouseEvent &event) {
    SetBackgroundColour(m_background);
    Refresh();
}
