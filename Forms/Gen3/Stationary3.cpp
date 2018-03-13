/*
 * This file is part of PokéFinder
 * Copyright (C) 2017 by Admiral_Fish and bumba
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "Stationary3.hpp"
#include "ui_Stationary3.h"

Stationary3::Stationary3(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Stationary3)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);

    ui->label->setVisible(false);
    ui->comboBoxShadow->setVisible(false);

    setupModels();
    updateProfiles();

    qRegisterMetaType<vector<Frame3>>("vector<Frame3>");
    connect(this, SIGNAL(updateView(vector<Frame3>)), this, SLOT(updateViewSearcher(vector<Frame3>)));
}

Stationary3::~Stationary3()
{
    delete ui;
    delete s;
    delete g;
}

void Stationary3::changeEvent(QEvent *event)
{
    if (event != NULL)
    {
        switch (event->type())
        {
            case QEvent::LanguageChange:
                ui->retranslateUi(this);
                break;
            default:
                break;
        }
    }
}

void Stationary3::setupModels()
{
    ui->tableViewGenerator->setModel(g);
    ui->tableViewGenerator->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableViewSearcher->setModel(s);
    ui->tableViewSearcher->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->initialSeedGenerator->setValues(0, 32, false);
    ui->idGenerator->setValues(0, 48, true);
    ui->sidGenerator->setValues(0, 48, true);
    ui->startingFrameGenerator->setValues(1, 32, true);
    ui->maxResultsGenerator->setValues(1, 32, true);
    ui->delayGenerator->setValues(1, 32, true);

    ui->idSearcher->setValues(0, 48, true);
    ui->sidSearcher->setValues(0, 48, true);

    ui->comboBoxMethodGenerator->setItemData(0, Method1);
    ui->comboBoxMethodGenerator->setItemData(1, Method2);
    ui->comboBoxMethodGenerator->setItemData(2, Method4);
    ui->comboBoxMethodGenerator->setItemData(3, XDColo);
    ui->comboBoxMethodGenerator->setItemData(4, Channel);

    ui->comboBoxMethodSearcher->setItemData(0, Method1);
    ui->comboBoxMethodSearcher->setItemData(1, Method2);
    ui->comboBoxMethodSearcher->setItemData(2, Method4);
    ui->comboBoxMethodSearcher->setItemData(3, XDColo);
    ui->comboBoxMethodSearcher->setItemData(4, XD);
    ui->comboBoxMethodSearcher->setItemData(5, Colo);
    ui->comboBoxMethodSearcher->setItemData(6, Channel);

    ui->comboBoxNatureGenerator->setup();
    ui->comboBoxNatureSearcher->setup();

    ui->comboBoxHiddenPowerGenerator->setup();
    ui->comboBoxHiddenPowerSearcher->setup();
}

void Stationary3::on_saveProfileGenerator_clicked()
{
    ProfileManager3* manager = new ProfileManager3();
    manager->setAttribute(Qt::WA_QuitOnClose, false);
    connect(manager, SIGNAL(updateProfiles()), this, SLOT(refreshProfiles()));
    manager->show();
}

void Stationary3::on_saveSearcher_clicked()
{
    ProfileManager3* manager = new ProfileManager3();
    manager->setAttribute(Qt::WA_QuitOnClose, false);
    connect(manager, SIGNAL(updateProfiles()), this, SLOT(refreshProfiles()));
    manager->show();
}

void Stationary3::refreshProfiles()
{
    emit alertProfiles(3);
}

void Stationary3::updateProfiles()
{
    profiles = Profile3::loadProfileList();

    QStandardItemModel *profile = new QStandardItemModel((int)profiles.size() + 1, 1, this);
    QStandardItem* firstProfile = new QStandardItem(tr("None"));
    profile->setItem(0, firstProfile);
    for(int i = 0; i < (int)profiles.size(); i++)
    {
        QStandardItem* item = new QStandardItem(profiles.at(i).profileName);
        profile->setItem(i + 1, item);
    }
    ui->comboBoxProfiles->setModel(profile);
}

void Stationary3::on_comboBoxProfiles_currentIndexChanged(int index)
{
    if(index == 0)
    {
        ui->idGenerator->setText("12345");
        ui->sidGenerator->setText("54321");
        ui->idSearcher->setText("12345");
        ui->sidSearcher->setText("54321");
    }
    else
    {
        ui->idGenerator->setText(QString::number(profiles.at(index - 1).tid));
        ui->sidGenerator->setText(QString::number(profiles.at(index - 1).sid));
        ui->idSearcher->setText(QString::number(profiles.at(index - 1).tid));
        ui->sidSearcher->setText(QString::number(profiles.at(index - 1).sid));
    }
}

void Stationary3::on_anyNatureGenerator_clicked()
{
    ui->comboBoxNatureGenerator->uncheckAll();
}

void Stationary3::on_anyHiddenPowerGenerator_clicked()
{
    ui->comboBoxHiddenPowerGenerator->uncheckAll();
}

void Stationary3::on_anyNatureSearcher_clicked()
{
    ui->comboBoxNatureSearcher->uncheckAll();
}

void Stationary3::on_anyHiddenPowerSearcher_clicked()
{
    ui->comboBoxHiddenPowerSearcher->uncheckAll();
}

void Stationary3::updateViewSearcher(vector<Frame3> frames)
{
    s->addItems(frames);
}

void Stationary3::on_checkBoxDelayGenerator_clicked()
{
    if (ui->checkBoxDelayGenerator->isChecked())
    {
        ui->delayGenerator->setEnabled(true);
    }
    else
    {
        ui->delayGenerator->setEnabled(false);
        ui->delayGenerator->setText("");
    }
}

void Stationary3::on_generate_clicked()
{
    g->clear();

    u32 seed = ui->initialSeedGenerator->text().toUInt(NULL, 16);
    u32 startingFrame = ui->startingFrameGenerator->text().toUInt(NULL, 10);
    u32 maxResults = ui->maxResultsGenerator->text().toUInt(NULL, 10);
    u32 tid = ui->idGenerator->text().toUInt(NULL, 10);
    u32 sid = ui->sidGenerator->text().toUInt(NULL, 10);
    u32 offset = 0;
    if(ui->checkBoxDelayGenerator->isChecked())
        offset = ui->delayGenerator->text().toUInt(NULL, 10);

    int genderRatioIndex = ui->comboBoxGenderRatioGenerator->currentIndex();
    Generator3 generator = Generator3(maxResults, startingFrame, seed, tid, sid, offset);
    FrameCompare compare = FrameCompare(ui->ivFilterGenerator->getEvals(), ui->ivFilterGenerator->getValues(),
                                        ui->comboBoxGenderGenerator->currentIndex(), genderRatioIndex, ui->comboBoxAbilityGenerator->currentIndex(),
                                        ui->comboBoxNatureGenerator->getChecked(), ui->comboBoxHiddenPowerGenerator->getChecked(),
                                        ui->checkBoxShinyGenerator->isChecked(), ui->checkBoxDisableGenerator->isChecked());

    generator.setup((Method)ui->comboBoxMethodGenerator->currentData().toInt(NULL));

    vector<Frame3> frames = generator.generate(compare);
    g->setModel(frames);
}

void Stationary3::search()
{
    isSearching = true;
    u32 tid = ui->idSearcher->text().toUInt(NULL, 10);
    u32 sid = ui->sidSearcher->text().toUInt(NULL, 10);

    int genderRatioIndex = ui->comboBoxGenderRatioSearcher->currentIndex();
    Searcher3 searcher = Searcher3(tid, sid, genderRatioIndex);
    FrameCompare compare = FrameCompare(ui->ivFilterSearcher->getEvals(), ui->ivFilterSearcher->getValues(), ui->comboBoxGenderSearcher->currentIndex(),
                                        genderRatioIndex, ui->comboBoxAbilitySearcher->currentIndex(), ui->comboBoxNatureSearcher->getChecked(),
                                        ui->comboBoxHiddenPowerSearcher->getChecked(), ui->checkBoxShinySearcher->isChecked(), false);

    searcher.setup((Method)ui->comboBoxMethodSearcher->currentData().toInt(NULL));
    if (searcher.frameType == XD || searcher.frameType == Colo)
        searcher.setupNatureLock(ui->comboBoxShadow->currentIndex());

    vector<u32> min = ui->ivFilterSearcher->getLower();
    vector<u32> max = ui->ivFilterSearcher->getUpper();

    for (u32 a = min[0]; a <= max[0]; a++)
    {
        for (u32 b = min[1]; b <= max[1]; b++)
        {
            for (u32 c = min[2]; c <= max[2]; c++)
            {
                for (u32 d = min[3]; d <= max[3]; d++)
                {
                    for (u32 e = min[4]; e <= max[4]; e++)
                    {
                        for (u32 f = min[5]; f <= max[5]; f++)
                        {
                            vector<Frame3> frames = searcher.search(a, b, c, d, e, f, compare);

                            if (!frames.empty())
                                emit updateView(frames);
                        }
                    }
                }
            }
        }
    }
    isSearching = false;
}

void Stationary3::on_search_clicked()
{
    if (isSearching == true)
        return;
    s->clear();
    s->setMethod((Method)ui->comboBoxMethodSearcher->currentData().toInt(NULL));
    std::thread job(&Stationary3::search, this);
    job.detach();
}

void Stationary3::on_comboBoxMethodSearcher_currentIndexChanged(int index)
{
    (void) index;
    Method method = (Method)ui->comboBoxMethodSearcher->currentData().toInt(NULL);
    ui->comboBoxShadow->clear();

    if (method == XD)
    {
        QStringList s;
        s << tr("Altaria") << tr("Arbok") << tr("Banette");
        s << tr("Butterfree") << tr("Chansey") << tr("Delcatty");
        s << tr("Dodrio") << tr("Dragonite") << tr("Dugtrio");
        s << tr("Duskull") << tr("Electabuzz") << tr("Farfetch'd");
        s << tr("Golduck") << tr("Grimer") << tr("Growlithe");
        s << tr("Gulpin (Citadark)") << tr("Gulpin (Initial)") << tr("Gulpin (Phenac)");
        s << tr("Hitmonchan") << tr("Hitmonlee") << tr("Hypno");
        s << tr("Kangaskhan") << tr("Lapras") << tr("Ledyba");
        s << tr("Lickitung") << tr("Lunatone") << tr("Magcargo");
        s << tr("Magmar") << tr("Magneton") << tr("Makuhita");
        s << tr("Manectric") << tr("Marowak") << tr("Mawile");
        s << tr("Meowth") << tr("Mr. Mime") << tr("Natu");
        s << tr("Nosepass") << tr("Numel") << tr("Paras");
        s << tr("Pidgeotto") << tr("Pineco") << tr("Pinsir");
        s << tr("Poliwrath") << tr("Poochyena") << tr("Primeape");
        s << tr("Ralts") << tr("Rapidash") << tr("Raticate");
        s << tr("Roselia") << tr("Sableye") << tr("Salamence");
        s << tr("Scyther") <<tr("Seedot (Citadark)") << tr("Seedot (Initial)");
        s << tr("Seedot (Phenac)") << tr("Seel") << tr("Shroomish");
        s << tr("Snorlax") << tr("Snorunt") << tr("Solrock");
        s << tr("Spearow") << tr("Spheal (Citadark)") << tr("Spheal (Initial)");
        s << tr("Spheal (Phenac)") << tr("Spinarak") << tr("Starmie");
        s << tr("Swinub") << tr("Tangela") << tr("Venomoth");
        s << tr("Voltorb") << tr("Vulpix") << tr("Weepinbell");

        ui->comboBoxShadow->addItems(s);
        ui->comboBoxShadow->setVisible(true);
        ui->label->setVisible(true);
    }
    else if (method == Colo)
    {
        QStringList s;
        s << tr("Gligar") << tr("Heracross") << tr("Makuhita") << tr("Murkrow") << tr("Ursaring");

        ui->comboBoxShadow->addItems(s);
        ui->comboBoxShadow->setVisible(true);
        ui->label->setVisible(true);
    }
    else
    {
        ui->comboBoxShadow->setVisible(false);
        ui->label->setVisible(false);
    }
}
