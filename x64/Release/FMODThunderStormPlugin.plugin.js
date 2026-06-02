studio.plugins.registerPluginDescription("ThunderStorm", {
    companyName: "Pandd",
    productName: "Procedural ThunderStorm",
    
    parameters: {
        "Intensity": { displayName: "Str." },
        "Indoor/Outdoor": { displayName: "Indr" },
        "Auto Thunder": { displayName: "Auto" },
        "Distance": { displayName: "Dist" },
        "Thunder": { displayName: "Trig" },
        "Rain Volume": { displayName: "Rain" },
        "Thunder Volume": { displayName: "Thdr" },
        
        "Thunder Strike": { displayName: "Strk" },
        "Thunder Rumble": { displayName: "Rmbl" },
        "Thunder Growl": { displayName: "Grwl" },
        
        "Rumble Level": { displayName: "Lvl" },
        "Rumble LoCut": { displayName: "LoCut" },
        "Rumble HiCut": { displayName: "HiCut" },
        
        "Shower Level": { displayName: "Lvl" },
        "Shower LoCut": { displayName: "LoCut" },
        "Shower HiCut": { displayName: "HiCut" },
        
        "L1 Level": { displayName: "Lvl" },
        "L1 Density": { displayName: "Dens" },
        "L1 Color": { displayName: "Colr" },
        "L1 Viscosity": { displayName: "Visc" },
        "L1 Min Vol": { displayName: "Min" },
        "L1 Max Vol": { displayName: "Max" },
        "L1 LoCut": { displayName: "LoCut" },
        "L1 HiCut": { displayName: "HiCut" },
        
        "L2 Level": { displayName: "Lvl" },
        "L2 Density": { displayName: "Dens" },
        "L2 Color": { displayName: "Colr" },
        "L2 Viscosity": { displayName: "Visc" },
        "L2 Min Vol": { displayName: "Min" },
        "L2 Max Vol": { displayName: "Max" },
        "L2 LoCut": { displayName: "LoCut" },
        "L2 HiCut": { displayName: "HiCut" }
    },
    
    deckUi: {
        deckWidgetType: studio.ui.deckWidgetType.Layout,
        layout: studio.ui.layoutType.HBoxLayout,
        contentsMargins: { left: 4, top: 4, right: 4, bottom: 4 },
        spacing: 8,
        items: [
            // --- GLOBAL / ENV PANEL ---
            {
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.VBoxLayout,
                isFramed: true,
                spacing: 2,
                contentsMargins: { left: 4, top: 2, right: 4, bottom: 2 },
                items: [
                    { deckWidgetType: studio.ui.deckWidgetType.Label, text: "GLOBAL" },
                    {
                        deckWidgetType: studio.ui.deckWidgetType.Layout,
                        layout: studio.ui.layoutType.HBoxLayout,
                        spacing: 4,
                        items: [
                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Intensity", label: "Str.", color: "#95A5A6", minimumWidth: 40 },
                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Indoor/Outdoor", label: "Indr", color: "#7F8C8D", minimumWidth: 40 }
                        ]
                    }
                ]
            },
            // --- RUMBLE PANEL ---
            {
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.VBoxLayout,
                isFramed: true,
                spacing: 2,
                contentsMargins: { left: 4, top: 2, right: 4, bottom: 2 },
                items: [
                    { deckWidgetType: studio.ui.deckWidgetType.Label, text: "RUMBLE" },
                    {
                        deckWidgetType: studio.ui.deckWidgetType.Layout,
                        layout: studio.ui.layoutType.HBoxLayout,
                        spacing: 12,
                        items: [
                            { 
                                deckWidgetType: studio.ui.deckWidgetType.Fader, 
                                binding: "Rumble Level",
                                color: "#A0522D",
                                minimumWidth: 36,
                                minimumHeight: 56
                            },
                            {
                                deckWidgetType: studio.ui.deckWidgetType.Layout,
                                layout: studio.ui.layoutType.VBoxLayout,
                                spacing: 4,
                                items: [
                                    { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Rumble LoCut", color: "#A0522D", minimumWidth: 56, minimumHeight: 56 },
                                    { deckWidgetType: studio.ui.deckWidgetType.Label, text: "LoCut" }
                                ]
                            },
                            {
                                deckWidgetType: studio.ui.deckWidgetType.Layout,
                                layout: studio.ui.layoutType.VBoxLayout,
                                spacing: 4,
                                items: [
                                    { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Rumble HiCut", color: "#CD853F", minimumWidth: 56, minimumHeight: 56 },
                                    { deckWidgetType: studio.ui.deckWidgetType.Label, text: "HiCut" }
                                ]
                            }
                        ]
                    }
                ]
            },
            // --- SHOWER PANEL ---
            {
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.VBoxLayout,
                isFramed: true,
                spacing: 2,
                contentsMargins: { left: 4, top: 2, right: 4, bottom: 2 },
                items: [
                    { deckWidgetType: studio.ui.deckWidgetType.Label, text: "SHOWER" },
                    {
                        deckWidgetType: studio.ui.deckWidgetType.Layout,
                        layout: studio.ui.layoutType.HBoxLayout,
                        spacing: 12,
                        items: [
                            { 
                                deckWidgetType: studio.ui.deckWidgetType.Fader, 
                                binding: "Shower Level",
                                color: "#2E8B57",
                                minimumWidth: 36,
                                minimumHeight: 56
                            },
                            {
                                deckWidgetType: studio.ui.deckWidgetType.Layout,
                                layout: studio.ui.layoutType.VBoxLayout,
                                spacing: 4,
                                items: [
                                    { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Shower LoCut", color: "#2E8B57", minimumWidth: 56, minimumHeight: 56 },
                                    { deckWidgetType: studio.ui.deckWidgetType.Label, text: "LoCut" }
                                ]
                            },
                            {
                                deckWidgetType: studio.ui.deckWidgetType.Layout,
                                layout: studio.ui.layoutType.VBoxLayout,
                                spacing: 4,
                                items: [
                                    { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Shower HiCut", color: "#3CB371", minimumWidth: 56, minimumHeight: 56 },
                                    { deckWidgetType: studio.ui.deckWidgetType.Label, text: "HiCut" }
                                ]
                            }
                        ]
                    }
                ]
            },
            // --- LIQUID 1 PANEL ---
            {
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.VBoxLayout,
                isFramed: true,
                spacing: 2,
                contentsMargins: { left: 4, top: 2, right: 4, bottom: 2 },
                items: [
                    { deckWidgetType: studio.ui.deckWidgetType.Label, text: "LIQUID 1" },
                    {
                        deckWidgetType: studio.ui.deckWidgetType.Layout,
                        layout: studio.ui.layoutType.HBoxLayout,
                        spacing: 4,
                        items: [
                            { 
                                deckWidgetType: studio.ui.deckWidgetType.Fader, 
                                binding: "L1 Level",
                                color: "#00BFFF",
                                minimumWidth: 24,
                                minimumHeight: 48
                            },
                            {
                                deckWidgetType: studio.ui.deckWidgetType.Layout,
                                layout: studio.ui.layoutType.VBoxLayout,
                                spacing: 4,
                                items: [
                                    {
                                        deckWidgetType: studio.ui.deckWidgetType.Layout,
                                        layout: studio.ui.layoutType.HBoxLayout,
                                        spacing: 4,
                                        items: [
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L1 Density", label: "Dens", color: "#00BFFF", minimumWidth: 40 },
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L1 Color", label: "Colr", color: "#00BFFF", minimumWidth: 40 },
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L1 Viscosity", label: "Visc", color: "#00BFFF", minimumWidth: 40 }
                                        ]
                                    },
                                    {
                                        deckWidgetType: studio.ui.deckWidgetType.Layout,
                                        layout: studio.ui.layoutType.HBoxLayout,
                                        spacing: 4,
                                        items: [
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L1 Min Vol", label: "Min", color: "#00BFFF", minimumWidth: 40 },
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L1 Max Vol", label: "Max", color: "#1E90FF", minimumWidth: 40 },
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L1 LoCut", label: "LoCut", color: "#1E90FF", minimumWidth: 40 },
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L1 HiCut", label: "HiCut", color: "#1E90FF", minimumWidth: 40 }
                                        ]
                                    }
                                ]
                            }
                        ]
                    }
                ]
            },
            // --- LIQUID 2 PANEL ---
            {
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.VBoxLayout,
                isFramed: true,
                spacing: 2,
                contentsMargins: { left: 4, top: 2, right: 4, bottom: 2 },
                items: [
                    { deckWidgetType: studio.ui.deckWidgetType.Label, text: "LIQUID 2" },
                    {
                        deckWidgetType: studio.ui.deckWidgetType.Layout,
                        layout: studio.ui.layoutType.HBoxLayout,
                        spacing: 4,
                        items: [
                            { 
                                deckWidgetType: studio.ui.deckWidgetType.Fader, 
                                binding: "L2 Level",
                                color: "#48D1CC",
                                minimumWidth: 24,
                                minimumHeight: 48
                            },
                            {
                                deckWidgetType: studio.ui.deckWidgetType.Layout,
                                layout: studio.ui.layoutType.VBoxLayout,
                                spacing: 4,
                                items: [
                                    {
                                        deckWidgetType: studio.ui.deckWidgetType.Layout,
                                        layout: studio.ui.layoutType.HBoxLayout,
                                        spacing: 4,
                                        items: [
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L2 Density", label: "Dens", color: "#48D1CC", minimumWidth: 40 },
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L2 Color", label: "Colr", color: "#48D1CC", minimumWidth: 40 },
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L2 Viscosity", label: "Visc", color: "#48D1CC", minimumWidth: 40 }
                                        ]
                                    },
                                    {
                                        deckWidgetType: studio.ui.deckWidgetType.Layout,
                                        layout: studio.ui.layoutType.HBoxLayout,
                                        spacing: 4,
                                        items: [
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L2 Min Vol", label: "Min", color: "#48D1CC", minimumWidth: 40 },
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L2 Max Vol", label: "Max", color: "#20B2AA", minimumWidth: 40 },
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L2 LoCut", label: "LoCut", color: "#20B2AA", minimumWidth: 40 },
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L2 HiCut", label: "HiCut", color: "#20B2AA", minimumWidth: 40 }
                                        ]
                                    }
                                ]
                            }
                        ]
                    }
                ]
            },
            // --- THUNDER PANEL ---
            {
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.VBoxLayout,
                isFramed: true,
                spacing: 2,
                contentsMargins: { left: 4, top: 2, right: 4, bottom: 2 },
                items: [
                    { deckWidgetType: studio.ui.deckWidgetType.Label, text: "THUNDER" },
                    {
                        deckWidgetType: studio.ui.deckWidgetType.Layout,
                        layout: studio.ui.layoutType.VBoxLayout,
                        spacing: 4,
                        items: [
                            {
                                deckWidgetType: studio.ui.deckWidgetType.Layout,
                                layout: studio.ui.layoutType.HBoxLayout,
                                spacing: 4,
                                items: [
                                    { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Distance", label: "Dist", color: "#FFA500", minimumWidth: 40 },
                                    { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Auto Thunder", label: "Auto", color: "#FF8C00", minimumWidth: 40 },
                                    { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Thunder Strike", label: "Strk", color: "#FF7F50", minimumWidth: 40 }
                                ]
                            },
                            {
                                deckWidgetType: studio.ui.deckWidgetType.Layout,
                                layout: studio.ui.layoutType.HBoxLayout,
                                spacing: 4,
                                items: [
                                    { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Thunder Rumble", label: "Rmbl", color: "#FF6347", minimumWidth: 40 },
                                    { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Thunder Growl", label: "Grwl", color: "#FF4500", minimumWidth: 40 },
                                    { deckWidgetType: studio.ui.deckWidgetType.Button, binding: "Thunder", label: "Trig" }
                                ]
                            }
                        ]
                    }
                ]
            },
            // --- MIXER PANEL ---
            {
                deckWidgetType: studio.ui.deckWidgetType.Layout,
                layout: studio.ui.layoutType.VBoxLayout,
                isFramed: true,
                spacing: 2,
                contentsMargins: { left: 4, top: 2, right: 4, bottom: 2 },
                items: [
                    { deckWidgetType: studio.ui.deckWidgetType.Label, text: "MIXER" },
                    {
                        deckWidgetType: studio.ui.deckWidgetType.Layout,
                        layout: studio.ui.layoutType.HBoxLayout,
                        spacing: 6,
                        items: [
                            { 
                                deckWidgetType: studio.ui.deckWidgetType.Fader, 
                                color: "#00BFFF",
                                binding: "Rain Volume", 
                                minimumWidth: 26,
                                minimumHeight: 48
                            },
                            { 
                                deckWidgetType: studio.ui.deckWidgetType.Fader, 
                                color: "#FF4500",
                                binding: "Thunder Volume", 
                                minimumWidth: 26,
                                minimumHeight: 48
                            }
                        ]
                    }
                ]
            }
        ]
    }
});
