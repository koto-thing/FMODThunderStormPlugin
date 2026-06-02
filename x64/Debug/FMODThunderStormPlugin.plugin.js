studio.plugins.registerPluginDescription("ThunderStorm", {
    companyName: "Pandd",
    productName: "Procedural ThunderStorm",
    
    parameters: {
        "Intensity": { displayName: "Intensity" },
        "Indoor/Outdoor": { displayName: "Indoor" },
        "Auto Thunder": { displayName: "Auto Rate" },
        "Distance": { displayName: "Distance" },
        "Thunder": { displayName: "Trigger" },
        "Rain Volume": { displayName: "Rain Vol" },
        "Thunder Volume": { displayName: "Thunder Vol" },
        
        "Thunder Strike": { displayName: "T Strike" },
        "Thunder Rumble": { displayName: "T Rumble" },
        "Thunder Growl": { displayName: "T Growl" },
        
        "Rumble Level": { displayName: "Rumble Level" },
        "Rumble LoCut": { displayName: "Rumble LoCut" },
        "Rumble HiCut": { displayName: "Rumble HiCut" },
        
        "Shower Level": { displayName: "Shower Level" },
        "Shower LoCut": { displayName: "Shower LoCut" },
        "Shower HiCut": { displayName: "Shower HiCut" },
        
        "L1 Level": { displayName: "L1 Level" },
        "L1 Density": { displayName: "L1 Density" },
        "L1 Color": { displayName: "L1 Color" },
        "L1 Viscosity": { displayName: "L1 Viscosity" },
        "L1 Min Vol": { displayName: "L1 Min Vol" },
        "L1 Max Vol": { displayName: "L1 Max Vol" },
        "L1 LoCut": { displayName: "L1 LoCut" },
        "L1 HiCut": { displayName: "L1 HiCut" },
        
        "L2 Level": { displayName: "L2 Level" },
        "L2 Density": { displayName: "L2 Density" },
        "L2 Color": { displayName: "L2 Color" },
        "L2 Viscosity": { displayName: "L2 Viscosity" },
        "L2 Min Vol": { displayName: "L2 Min Vol" },
        "L2 Max Vol": { displayName: "L2 Max Vol" },
        "L2 LoCut": { displayName: "L2 LoCut" },
        "L2 HiCut": { displayName: "L2 HiCut" }
    },
    
    deckUi: {
        deckWidgetType: studio.ui.deckWidgetType.Layout,
        layout: studio.ui.layoutType.HBoxLayout,
        contentsMargins: { left: 4, top: 4, right: 4, bottom: 4 },
        spacing: 4,
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
                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Indoor/Outdoor", label: "Indoor", color: "#7F8C8D", minimumWidth: 40 }
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
                        spacing: 4,
                        items: [
                            { 
                                deckWidgetType: studio.ui.deckWidgetType.Fader, 
                                binding: "Rumble Level",
                                color: "#A0522D",
                                minimumWidth: 24,
                                minimumHeight: 48
                            },
                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Rumble LoCut", label: "LoCut", color: "#A0522D", minimumWidth: 40 },
                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Rumble HiCut", label: "HiCut", color: "#CD853F", minimumWidth: 40 }
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
                        spacing: 4,
                        items: [
                            { 
                                deckWidgetType: studio.ui.deckWidgetType.Fader, 
                                binding: "Shower Level",
                                color: "#2E8B57",
                                minimumWidth: 24,
                                minimumHeight: 48
                            },
                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Shower LoCut", label: "LoCut", color: "#2E8B57", minimumWidth: 40 },
                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Shower HiCut", label: "HiCut", color: "#3CB371", minimumWidth: 40 }
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
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L1 Density", label: "Dens.", color: "#00BFFF", minimumWidth: 40 },
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L1 Color", label: "Color", color: "#00BFFF", minimumWidth: 40 },
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L1 Viscosity", label: "Visc.", color: "#00BFFF", minimumWidth: 40 }
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
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L2 Density", label: "Dens.", color: "#48D1CC", minimumWidth: 40 },
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L2 Color", label: "Color", color: "#48D1CC", minimumWidth: 40 },
                                            { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "L2 Viscosity", label: "Visc.", color: "#48D1CC", minimumWidth: 40 }
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
                                    { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Distance", label: "Dist.", color: "#FFA500", minimumWidth: 40 },
                                    { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Auto Thunder", label: "Auto", color: "#FF8C00", minimumWidth: 40 },
                                    { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Thunder Strike", label: "Strike", color: "#FF7F50", minimumWidth: 40 }
                                ]
                            },
                            {
                                deckWidgetType: studio.ui.deckWidgetType.Layout,
                                layout: studio.ui.layoutType.HBoxLayout,
                                spacing: 4,
                                items: [
                                    { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Thunder Rumble", label: "Rumble", color: "#FF6347", minimumWidth: 40 },
                                    { deckWidgetType: studio.ui.deckWidgetType.Dial, binding: "Thunder Growl", label: "Growl", color: "#FF4500", minimumWidth: 40 },
                                    { deckWidgetType: studio.ui.deckWidgetType.Button, binding: "Thunder", label: "Trigger" }
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
